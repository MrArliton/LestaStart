// Fill out your copyright notice in the Description page of Project Settings.


#include "LivingAttributeComponent.h"
#include "Net/UnrealNetwork.h"

ULivingAttributeComponent::ULivingAttributeComponent()
{
	SetIsReplicatedByDefault(false);
	PrimaryComponentTick.bCanEverTick = false;
}

void ULivingAttributeComponent::BeginPlay()
{
	Super::BeginPlay();	

	Health = MaxHealth;
}

void ULivingAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ULivingAttributeComponent, Health);
}

void  ULivingAttributeComponent::OnRep_Health()
{
	OnChangeHealth.Broadcast(Health);
	if (Health <= 0.0f && !IsDeath)
	{
		IsDeath = true;
		OnDeath.Broadcast();
	}
	else
	{
		IsDeath = false;
	}
}

void ULivingAttributeComponent::Heal(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Health = FMath::Clamp(Health + Value, 0, MaxHealth);
		if (!IsDeath)
		{
			OnChangeHealth.Broadcast(Health);
		}
		if (Value > 0.0f) /** Character is now alive */
		{
			IsDeath = false;
		}
	}
}

void ULivingAttributeComponent::Damage(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Health = FMath::Clamp(Health - Value, 0, MaxHealth);
		if (!IsDeath)
		{
			OnChangeHealth.Broadcast(Health);
			if (Health == 0)
			{
				OnDeath.Broadcast();
				IsDeath = true;
			}
		}
	}
}


bool ULivingAttributeComponent::IsFullHealth() const
{
	return Health == MaxHealth;
}


float ULivingAttributeComponent::GetMaxHealth()  const
{
	return MaxHealth;
}

float ULivingAttributeComponent::GetCurrentHealth() const
{
	return Health;
}