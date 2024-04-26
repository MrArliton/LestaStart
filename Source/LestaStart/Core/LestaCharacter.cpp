// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h" 
#include "../Components/Attributes/LivingAttributeComponent.h"

ALestaCharacter::ALestaCharacter()
{
	NetUpdateFrequency = 10.f;

	USkeletalMeshComponent* SkeletalMesh = GetMesh();

	if (IsValid(SkeletalMesh))
	{
		// Camera
		CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
		CameraComponent->bUsePawnControlRotation = true; // Camera rotation is synchronized with Player Controller rotation
		CameraComponent->SetupAttachment(SkeletalMesh);
	}

	// Living Attribute Component - Functional components
	LivingAttributeComponent = CreateDefaultSubobject<ULivingAttributeComponent>(TEXT("LivingAttribute"));
	// Events 
	LivingAttributeComponent->OnDeath.AddDynamic(this, &ALestaCharacter::OnDeath);
	
	this->OnTakeAnyDamage.AddDynamic(this, &ALestaCharacter::OnDamagedAny);
}

void ALestaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
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
				CurrentWeapon->Deactivate(true);
			}
		}
		if (!Weapons.IsEmpty())
		{
			CurrentWeapon = Weapons[0];
			CurrentWeapon->Activate();
		}
	}
}

void ALestaCharacter::Tick(float DeltaTime)
{
	if (IsValid(GetMesh())) 
	{
		FRotator Socket = GetMesh()->GetSocketRotation(FName(TEXT("weapon_r")));
		UE_LOG(LogTemp, Warning, TEXT("Socket: %s"), *Socket.ToString());
	}
}

void ALestaCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void ALestaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
		// Print error message into log
		// You can read more here: https://dev.epicgames.com/documentation/en-us/unreal-engine/logging-in-unreal-engine
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
}

void ALestaCharacter::OnChangeWeaponInput(const FInputActionInstance& InputActionInstance)
{
	/** Cannot change weapon when we attack */
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
			CurrentWeaponId = Weapons.Num()-1;
		}
	}
	CurrentWeapon->Deactivate(true);
	CurrentWeapon = Weapons[CurrentWeaponId];
	CurrentWeapon->Activate();
	OnChangeWeapon.Broadcast(CurrentWeapon);
}
void ALestaCharacter::OnChangeWeaponNumberInput(const FInputActionInstance& InputActionInstance)
{
	/** Cannot change weapon when we attack */
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
	else if(CurrentWeaponId < 0)
	{
		CurrentWeaponId = Weapons.Num() - 1;
	}
	CurrentWeapon->Deactivate(true);
	CurrentWeapon = Weapons[CurrentWeaponId];
	CurrentWeapon->Activate();
	OnChangeWeapon.Broadcast(CurrentWeapon);
}

void ALestaCharacter::OnDamagedAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	LivingAttributeComponent->Damage(Damage);
}

void ALestaCharacter::OnDeath()
{
	Destroy();
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