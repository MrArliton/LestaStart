// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastWeaponComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UBlastWeaponComponent::UBlastWeaponComponent() : UBaseWeaponComponent()
{
	IgnoredActors.Add(GetOwner());

	BaseDamage = 100.0f;
	AttackDistance = 800.0f;
	PowerIPS = 0.1f;
}

void UBlastWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)  
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsAttacking) 
	{
		AccumulationState(DeltaTime);
	}
	else
	{
		ReleaseState(DeltaTime);
	}
}

void UBlastWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBlastWeaponComponent, CurrentPower)
}

void UBlastWeaponComponent::AccumulationState(float DeltaTime)
{
	// Accumulate power
	CurrentPower = FMath::Clamp(DeltaTime * PowerIPS + CurrentPower, 0, 1.0f);
}

void UBlastWeaponComponent::ReleaseState(float DeltaTime)
{
	// If have accumulated power, deal damage and demonstrate effects
	if (IsValid(DirectionComponent) && CurrentPower > 0.0f)
	{
		DrawDebugSphere(GetWorld(), DirectionComponent->GetComponentLocation(), AttackDistance, 20, FColor::Emerald);
		if (GetOwnerRole() == ROLE_Authority)
		{
			AActor* Owner = GetOwner();
			UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage * CurrentPower,
				DirectionComponent->GetComponentLocation(),
				AttackDistance, nullptr, IgnoredActors, Owner, nullptr, false, ECC_Pawn);

		}
		CurrentPower = 0.0f;
	}
}

float UBlastWeaponComponent::GetCurrentPower() const
{
	return CurrentPower;
}

void UBlastWeaponComponent::AddIgnoreActor(AActor* Actor)
{
	IgnoredActors.Add(Actor);
}

void UBlastWeaponComponent::RemoveIgnoreActor(AActor* Actor)
{
	IgnoredActors.Remove(Actor);
}