// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserWeaponComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ULaserWeaponComponent::ULaserWeaponComponent() : UBaseWeaponComponent()
{
	MaxAmmo = 3.0f;
}

void ULaserWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (LaserBeam && IsValid(DirectionComponent))
	{
		LaserNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(LaserBeam, DirectionComponent, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, false, false);
	}
}

void ULaserWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (IsAttacking)
	{
		FHitResult Result;
		// Damage Logic
		if (IsValid(DirectionComponent))
		{
			// Laser beam distance
			FVector BeamLength{ 0.0f };

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
			World->SweepSingleByChannel(Result, StartPosition, EndPostion, FRotator::ZeroRotator.Quaternion(), ECC_Pawn, FCollisionShape::MakeSphere(TraceSphereRadius));

			if (Result.bBlockingHit)
			{
				const float Damage = FMath::Max(0, DeltaTime * BaseDamage * (1 - Result.Distance / AttackDistance)); // Damage depends on distance (Linear)
				UGameplayStatics::ApplyDamage(Result.GetActor(), Damage, GetWorld()->GetFirstPlayerController(), GetOwner(), nullptr);
			}
			// VFX Logic
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
			}
		}
		else // Direction Component is invalid
		{
			EndAttack();

			UE_LOG(LogInput, Error, TEXT("DirectionComponent is undefined for: %s"), *GetFullNameSafe(this));
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
					IsOverheat = false;
				},
			this->ReloadTime, false);
		}
	}
	else
	{	
		if (!IsOverheat)
		{
			this->RestoreAmmo(DeltaTime * ChargingSpeed);
		}
	}


}

void ULaserWeaponComponent::StartAttack() 
{
	Super::StartAttack();

	if (IsAttacking)
	{
		if (IsValid(LaserNiagaraComponent))
		{
			LaserNiagaraComponent->Activate();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LaserNiagaraComponent is invalid, object: %s"), *GetName());
		}
	}
}

void ULaserWeaponComponent::EndAttack() 
{
	Super::EndAttack();

	if (IsValid(LaserNiagaraComponent)) 
	{
		LaserNiagaraComponent->Deactivate();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LaserNiagaraComponent is invalid, object: %s"), *GetName());
	}
}
