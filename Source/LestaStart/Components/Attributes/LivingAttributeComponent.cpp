// Fill out your copyright notice in the Description page of Project Settings.


#include "LivingAttributeComponent.h"

// Sets default values for this component's properties
ULivingAttributeComponent::ULivingAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void ULivingAttributeComponent::BeginPlay()
{
	Super::BeginPlay();	

	Health = MaxHealth;
}


// Called every frame
void ULivingAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void ULivingAttributeComponent::Heal(float Value)
{
	Health = FMath::Clamp(Health + Value, 0, MaxHealth);
	if (!IsDeath)
	{
		OnChangeHealth.Broadcast(Health);
	}
	if (Value > 0.0f) // Character is now alive
	{
		IsDeath = false;
	}
}


void ULivingAttributeComponent::Damage(float Value)
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


bool ULivingAttributeComponent::HealthIsFull()
{
	return Health == MaxHealth;
}


float ULivingAttributeComponent::GetMaxHealth() 
{
	return MaxHealth;
}

float ULivingAttributeComponent::GetCurrentHealth() 
{
	return Health;
}