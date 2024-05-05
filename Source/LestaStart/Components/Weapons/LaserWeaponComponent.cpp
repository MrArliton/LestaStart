 // Fill out your copyright notice in the Description page of Project Settings.

#include "LaserWeaponComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

ULaserWeaponComponent::ULaserWeaponComponent() : UBaseWeaponComponent()
{
	MaxAmmo = 3.0f;
	BaseDamage = 30.0f;
	AttackDistance = 1500.0f;
	TraceSphereRadius = 5.0f;
	DischargingSpeed = 1.0f;
	ChargingSpeed = 1.0f;
}

void ULaserWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// We ignore our actor during tracing or attack
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		TraceParams.AddIgnoredActor(Owner);
	}

	// Don't create visual effects on dedicated server
	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		if (LaserBeam && IsValid(DirectionComponent))
		{
			LaserNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(LaserBeam, DirectionComponent, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, false, false);
			if (!IsValid(LaserNiagaraComponent))
			{
				UE_LOG(LogTemp, Warning, TEXT("Cannot spawn LaserNiagaraComponent, object: %s"), *GetName());
			}
		}
	}
}

void ULaserWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsAttacking && !IsOverheat)
	{
		ActiveState(DeltaTime);
	}
	else
	{	
		PendingState(DeltaTime);
	}
}

/** Attack, activate and correct effects */
void ULaserWeaponComponent::ActiveState(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FHitResult Result;
	// Damage Logic
	if (IsValid(DirectionComponent))
	{
		auto Transform = DirectionComponent->GetComponentTransform();
		// Start position
		const FVector StartPosition = Transform.GetLocation();
		// Some calculations
		const FRotator Rotation = Transform.Rotator();
		const float YawRadian = FMath::DegreesToRadians(Rotation.Yaw);
		const float PitchRadian = FMath::DegreesToRadians(Rotation.Pitch);
		const FVector Direction = FVector(FMath::Cos(YawRadian), FMath::Sin(YawRadian), FMath::Sin(PitchRadian));
		// End position
		const FVector EndPostion = StartPosition + Direction * AttackDistance;
		// Trace 
		World->SweepSingleByChannel(Result, StartPosition, EndPostion, FRotator::ZeroRotator.Quaternion(), ECC_Pawn, FCollisionShape::MakeSphere(TraceSphereRadius), TraceParams);
		Multicast_Debug(StartPosition);
		Multicast_Debug(EndPostion);

		if (GetOwnerRole() == ROLE_Authority && Result.bBlockingHit)
		{
			const float Damage = FMath::Max(0, DeltaTime * BaseDamage * (1 - Result.Distance / AttackDistance)); // Damage depends on distance (Linear)
			UGameplayStatics::ApplyDamage(Result.GetActor(), Damage, GetWorld()->GetFirstPlayerController(), GetOwner(), nullptr);
		}
		// Don't create visual effects on dedicated server
		if (GetNetMode() != ENetMode::NM_DedicatedServer)
		{
			// Laser effect
			FVector BeamLength{ 0.0f };
			if (IsValid(LaserNiagaraComponent))
			{
				if (Result.bBlockingHit)
				{
					BeamLength.X = Result.Distance; // Set the laser length to the distance to the object
					LaserNiagaraComponent->SetVariableInt(FName(TEXT("SparkEnable")), 1);
				}
				else
				{
					BeamLength.X = AttackDistance; // Set the laser length to the attack distance
					LaserNiagaraComponent->SetVariableInt(FName(TEXT("SparkEnable")), 0);
				}
				LaserNiagaraComponent->SetVariableVec3(FName(TEXT("BeamEnd")), BeamLength);

				if (!LaserNiagaraComponent->IsActive())
				{
					LaserNiagaraComponent->Activate();
				}
			}
		}
	}
	else // Direction Component is invalid
	{
		Server_EndAttack();

		UE_LOG(LogInput, Warning, TEXT("DirectionComponent is undefined for: %s"), *GetFullNameSafe(this));
	}
	// Ammo Logic
	this->ConsumeAmmo(DeltaTime * DischargingSpeed);
	// Overheat
	if (this->GetCurrentAmmo() == 0.0f)
	{
		IsOverheat = true;
		World->GetTimerManager().SetTimer(TimerOverheatHandle,
			[this]()
			{
				if(IsValid(this)) IsOverheat = false;
			},
			ReloadTime, false);
	}
}

/** Don't attack, disable effects and wait  */
void ULaserWeaponComponent::PendingState(float DeltaTime)
{
	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		if (IsValid(LaserNiagaraComponent))
		{
			LaserNiagaraComponent->Deactivate();
		}
	}

	if (!IsOverheat)
	{
		this->RestoreAmmo(DeltaTime * ChargingSpeed);
	}
}


void ULaserWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULaserWeaponComponent, IsOverheat)
}

void ULaserWeaponComponent::Multicast_Debug_Implementation(FVector Location) 
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		DrawDebugSphere(GetWorld(), Location, 10, 10, FColor::Red, false, 0.5f);
	}
	else 
	{
		DrawDebugSphere(GetWorld(), Location, 10, 10, FColor::Cyan, false, 0.5f);
	}
}