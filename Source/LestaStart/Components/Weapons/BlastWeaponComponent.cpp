// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastWeaponComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UBlastWeaponComponent::UBlastWeaponComponent() : UBaseWeaponComponent()
{
	IgnoredActors.Add(GetOwner());
}

void UBlastWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)  
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsAttacking) 
	{
		CurrentPower = FMath::Clamp(DeltaTime * PowerIPS + CurrentPower, 0, MaxPower);
	}
	else
	{
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
}

void UBlastWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBlastWeaponComponent, CurrentPower)
}

void UBlastWeaponComponent::AddIgnoreActor(AActor* Actor)
{
	IgnoredActors.Add(Actor);
}

void UBlastWeaponComponent::RemoveIgnoreActor(AActor* Actor)
{
	IgnoredActors.Remove(Actor);
}