// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h" 
#include "../Components/Attributes/LivingAttributeComponent.h"

ALestaCharacter::ALestaCharacter()
{
	NetUpdateFrequency = 10.f;
	bReplicates = true;
	SetReplicateMovement(true);

	USkeletalMeshComponent* SkeletalMesh = GetMesh();

	if (IsValid(SkeletalMesh))
	{
		// Camera
		CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
		CameraComponent->bUsePawnControlRotation = true; // Camera rotation is synchronized with Player Controller rotation
		CameraComponent->SetupAttachment(SkeletalMesh);
	}

	// Replicated Living Attribute Component
	LivingAttributeComponent = CreateDefaultSubobject<ULivingAttributeComponent>(TEXT("LivingAttribute"));
	if (LivingAttributeComponent)
	{
		LivingAttributeComponent->SetIsReplicated(true);
		LivingAttributeComponent->OnDeath.AddDynamic(this, &ALestaCharacter::OnDeath);
	}
	this->OnTakeAnyDamage.AddDynamic(this, &ALestaCharacter::OnDamagedAny);
}

void  ALestaCharacter::Tick(float DeltaTime)
{
	// Set anim rotation, Controll Rotation auto replicated to server, server update AnimRotation and replicate it on clients.   
	if (IsValid(GetController()))
	{
		AnimRotation = GetControlRotation();
	}
}

void ALestaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetRemoteRole() < ROLE_Authority)
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			return;
		}
		// Take input component for server local player
		UInputComponent* PlayerInputComponent = nullptr;
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (IsValid(PlayerController)) 
		{
			PlayerInputComponent = PlayerController->InputComponent;
		}

		for (TSubclassOf<ABaseWeapon> WeaponClass : WeaponClasses)
		{
			if (WeaponClass.Get())
			{
				FTransform WeaponSocketTransform = GetMesh()->GetSocketTransform(WeaponSocketName);
				AActor* Weapon = World->SpawnActor(WeaponClass, &WeaponSocketTransform);
				CurrentWeapon = Cast<ABaseWeapon>(Weapon);
				if (IsValid(CurrentWeapon))
				{
					Weapons.Add(CurrentWeapon);
					CurrentWeapon->AttachWeapon(this, WeaponSocketName);
					CurrentWeapon->SetupInputComponent(PlayerInputComponent);		
					CurrentWeapon->Deactivate(false);
				}
			}
			/** Activate first weapon */
			if (!Weapons.IsEmpty())
			{
				CurrentWeapon = Weapons[0];
				CurrentWeapon->Activate();
			}
		}
	}
}

void ALestaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALestaCharacter, LivingAttributeComponent)
	// Skip owner: Since owner is the source of the results, the server should not check it. 
	DOREPLIFETIME_CONDITION(ALestaCharacter, AnimRotation, ELifetimeCondition::COND_SkipOwner)
	// Owner Only: Other clients do not control the player's weapon, so they do not need up-to-date information about it.
	DOREPLIFETIME_CONDITION(ALestaCharacter, Weapons, ELifetimeCondition::COND_OwnerOnly)
	DOREPLIFETIME_CONDITION(ALestaCharacter, CurrentWeaponId, ELifetimeCondition::COND_OwnerOnly)
	DOREPLIFETIME_CONDITION(ALestaCharacter, CurrentWeapon, ELifetimeCondition::COND_OwnerOnly)
}

void ALestaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent = PlayerInputComponent;

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIC) // should validate component because developers can change input component class through Project Settings
	{
		EIC->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnMoveInput);
		EIC->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnLookInput);
		EIC->BindAction(ChangeWeaponInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnChangeWeaponInput);
		EIC->BindAction(ChangeWeaponByNumberInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnChangeWeaponNumberInput);
	}
	else
	{
		UE_LOG(LogInput, Error, TEXT("Unexpected input component class: %s"), *GetFullNameSafe(PlayerInputComponent))
	}
}

void ALestaCharacter::OnMoveInput(const FInputActionInstance& InputActionInstance)
{
	// Controller rotation Yaw determines which direction Character is facing
	// so MoveForward = along that direction AND MoveRight = to the right of that direction
	const float YawDegree = GetControlRotation().Yaw; // controller rotation Yaw (in degrees)
	const float YawRadian = FMath::DegreesToRadians(YawDegree); // controller rotation Yaw (in radians)
	const FVector ForwardDirection = FVector(FMath::Cos(YawRadian), FMath::Sin(YawRadian), 0.f);
	const FVector RightDirection = FVector(FMath::Cos(YawRadian + UE_HALF_PI), FMath::Sin(YawRadian + UE_HALF_PI), 0.f);

	const FVector2D Input2D = InputActionInstance.GetValue().Get<FVector2D>();

	AddMovementInput(ForwardDirection * Input2D.X + RightDirection * Input2D.Y);
}

void ALestaCharacter::OnLookInput(const FInputActionInstance& InputActionInstance)
{
	const FVector2D Input2D = InputActionInstance.GetValue().Get<FVector2D>();
	AddControllerYawInput(Input2D.X);
	AddControllerPitchInput(Input2D.Y);	

	AnimRotation = GetControlRotation();
}

void ALestaCharacter::OnChangeWeaponInput(const FInputActionInstance& InputActionInstance)
{
	/** Cannot change weapon when we attack */
	if (HasAuthority())
	{
		if (IsValid(CurrentWeapon) && CurrentWeapon->IsAttacking())
		{
			return;
		}

		float Value = InputActionInstance.GetValue().Get<float>();

		if (Value > 0.0f)
		{
			CurrentWeaponId++;
			if (CurrentWeaponId >= Weapons.Num())
			{
				CurrentWeaponId = 0;
			}
		}
		else
		{
			CurrentWeaponId--;
			if (CurrentWeaponId < 0)
			{
				CurrentWeaponId = Weapons.Num() - 1;
			}
		}

		CurrentWeapon->Deactivate(false);
		CurrentWeapon = Weapons[CurrentWeaponId];
		CurrentWeapon->Activate();
		OnChangeWeapon.Broadcast(CurrentWeapon);
	}

}
void ALestaCharacter::OnChangeWeaponNumberInput(const FInputActionInstance& InputActionInstance)
{
	/** Cannot change weapon when we attack */
	if (HasAuthority())
	{
		if (IsValid(CurrentWeapon) && CurrentWeapon->IsAttacking())
		{
			return;
		}

		int32 Value = static_cast<int>(InputActionInstance.GetValue().Get<float>());
		CurrentWeaponId = Value;
		if (CurrentWeaponId >= Weapons.Num())
		{
			CurrentWeaponId = 0;
		}
		else if (CurrentWeaponId < 0)
		{
			CurrentWeaponId = Weapons.Num() - 1;
		}
		CurrentWeapon->Deactivate(false);
		CurrentWeapon = Weapons[CurrentWeaponId];
		CurrentWeapon->Activate();
		OnChangeWeapon.Broadcast(CurrentWeapon);
	}
}

void ALestaCharacter::OnDamagedAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	LivingAttributeComponent->Damage(Damage);
}

void ALestaCharacter::OnDeath()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		for (ABaseWeapon* Weapon : Weapons)
		{
			if (IsValid(Weapon))
			{
				Weapon->DetachWeapon(true);
			}
		}
		Destroy();
	}
}

void ALestaCharacter::AttachWeapon(ABaseWeapon* Weapon)
{
	if(IsValid(Weapon))
	{
		Weapons.Add(Weapon);
		Weapon->AttachWeapon(this, WeaponSocketName);
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (IsValid(PlayerController))
		{
			Weapon->SetupInputComponent(PlayerController->InputComponent);			
		}
		Weapon->Deactivate(false);
	}
}

void ALestaCharacter::OnRep_Weapons(const TArray<ABaseWeapon*>& OldWeapons)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController))
	{
		for (ABaseWeapon* Weapon : Weapons)
		{
			// Check that weapon is new in array, and setup input components 
			if (IsValid(Weapon))
			{
				Weapon->SetupInputComponent(PlayerController->InputComponent);
				Weapon->AttachWeapon(this, WeaponSocketName);
			}
		}
	}
}

//** Getters *
ULivingAttributeComponent* ALestaCharacter::GetLivingAttributeComponent() 
{
	return LivingAttributeComponent;
}

ABaseWeapon* ALestaCharacter::GetCurrentWeaponActor()
{
	return CurrentWeapon;
}

FRotator ALestaCharacter::GetAnimationRotation()
{
	return AnimRotation;
}