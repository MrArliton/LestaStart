
#include "LestaCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h" 
#include "LestaStart/Components/Attributes/LivingAttributeComponent.h"
#include "LestaStart/Components/Utility/TraceEstimatorComponent.h"
ALestaCharacter::ALestaCharacter()
{
	bReplicates = true;
	NetUpdateFrequency = 10.f;
	SetReplicateMovement(true);

	USkeletalMeshComponent* SkeletalMesh = GetMesh();

	if (IsValid(SkeletalMesh))
	{
		// Camera
		CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
		if (CameraComponent)
		{
			CameraComponent->bUsePawnControlRotation = true; // Camera rotation is synchronized with Player Controller rotation
			CameraComponent->SetupAttachment(SkeletalMesh);	
		}

		// Animation Trace Component 
		AnimationTraceComponent = CreateDefaultSubobject<UTraceEstimatorComponent>(TEXT("Animation Trace Estimator"));
		if (AnimationTraceComponent)
		{
			AnimationTraceComponent->bAutoTrace = true;
			AnimationTraceComponent->SetupAttachment(SkeletalMesh, WeaponSocketName);
		}
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
		// Spawn and attach default player weapons.
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
			// Activate first weapon
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
	DOREPLIFETIME_CONDITION(ALestaCharacter, AnimationRotation, ELifetimeCondition::COND_SimulatedOnly)
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

	AnimationRotation = GetControlRotation();
}

void ALestaCharacter::OnChangeWeaponInput(const FInputActionInstance& InputActionInstance)
{
	/** Cannot change weapon when we attack */
	float Value = InputActionInstance.GetValue().Get<float>();

	if (Value > 0.0f)
	{
		ChangeWeapon(CurrentWeaponId + 1);
	}
	else
	{
		ChangeWeapon(CurrentWeaponId - 1);
	}
}

void ALestaCharacter::OnChangeWeaponNumberInput(const FInputActionInstance& InputActionInstance)
{
	int32 Value = static_cast<int>(InputActionInstance.GetValue().Get<float>());
	ChangeWeapon(Value);
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

void ALestaCharacter::AttachWeapon(ABaseWeapon* Weapon, bool Deactivate)
{
	if (HasAuthority())
	{
		if (IsValid(Weapon))
		{
			Weapons.Add(Weapon);
			Weapon->AttachWeapon(this, WeaponSocketName);

			APlayerController* PlayerController = Cast<APlayerController>(GetController());

			if (IsValid(PlayerController))
			{
				Weapon->SetupInputComponent(PlayerController->InputComponent);
			}
			else 
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot attach input component to weapon, player controller is undefined. Object: %s"), *GetName())
			}
			Weapon->Deactivate(false);
		}
	}
}

void ALestaCharacter::Server_ChangeWeapon_Implementation(int32 NewWeaponID)
{
	ChangeWeapon(NewWeaponID);
}

void ALestaCharacter::ChangeWeapon(int32 NewWeaponID)
{
	// Cannot change weapon, when player is attaking
	if (IsValid(CurrentWeapon) && CurrentWeapon->IsAttacking())
	{
		return;
	}

	if (HasAuthority())
	{	
		if (NewWeaponID >= Weapons.Num())
		{
			CurrentWeaponId = 0;
		}
		else if (NewWeaponID < 0)
		{
			CurrentWeaponId = Weapons.Num() - 1;
		}
		else 
		{
			CurrentWeaponId = NewWeaponID;
		}
		// Deactivate old weapon
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->Deactivate(false);
		}
		// Set new weapon
		CurrentWeapon = Weapons[CurrentWeaponId];
		// Activate new weapon
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->Activate();
		}
	}
	else 
	{
		Server_ChangeWeapon(NewWeaponID);
	}
	OnChangeWeapon.Broadcast(CurrentWeapon);
}

void ALestaCharacter::OnRep_Weapons()
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


void ALestaCharacter::UpdateAnimationRotation()
{
	// Get controll rotation (On clients, animation rotation replication occurs from the server)
	FRotator BufferRotator;
	if (IsValid(GetController()))
	{
		BufferRotator = GetControlRotation().GetNormalized();
	}
	else 
	{
		BufferRotator = AnimationRotation;
	}
	// Limiting animation rotation when colliding with a wall
	float RotationCorrectionValue = AnimationTraceComponent->GetLastEstimatedValue();

	if (BufferRotator.Pitch < RotationCorrectionValue)
	{
		BufferRotator.Pitch = RotationCorrectionValue;
	}
	AnimationRotation = BufferRotator;
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
	// Update AnimationRotation before return its value 
	// Update only on autonomous proxy and server
	if (GetRemoteRole() > ROLE_SimulatedProxy)
	{
		UpdateAnimationRotation();
	}
	return AnimationRotation;
}