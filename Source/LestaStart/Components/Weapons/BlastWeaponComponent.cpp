// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastWeaponComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

UBlastWeaponComponent::UBlastWeaponComponent() : UBaseWeaponComponent()
{
	IgnoredActors.Add(GetOwner());

	BaseDamage = 100.0f;
	AttackDistance = 800.0f;
	PowerIPS = 0.1f;
	ThresholdActivationPower = 0.2f;
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
	if (IsValid(DirectionComponent) && CurrentPower > ThresholdActivationPower)
	{
		// Damage
		if (GetOwnerRole() == ROLE_Authority)
		{
			UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage * CurrentPower,
				DirectionComponent->GetComponentLocation(),
				AttackDistance, nullptr, IgnoredActors, GetOwner(), nullptr, false, ECC_Pawn);
		}

		// --- Activate on server and owner client (if it's dedicated server, ActivateEffect does not create effect) 
		// Draw effect on another clients
		Multicast_ActivateEffect();
		// Draw effect locally 	
		ActivateEffect();
	}

	CurrentPower = 0.0f;
}

void UBlastWeaponComponent::Multicast_ActivateEffect_Implementation()
{
	// Create only on clients that have no control
	if(GetOwnerRole() == ROLE_SimulatedProxy)
	{
		ActivateEffect();
	}
}

void UBlastWeaponComponent::ActivateEffect()
{
	if (GetNetMode() > ENetMode::NM_DedicatedServer)
	{
		if (!UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BlastSystem, DirectionComponent->GetComponentLocation(), FRotator(0.f)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot spawn blast niagara component, object: %s"), *GetName());
		}
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