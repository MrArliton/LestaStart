// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeaponComponent.h"

UBaseWeaponComponent::UBaseWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UBaseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	Ammo = MaxAmmo;
}


// Called every frame
void UBaseWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBaseWeaponComponent::ConsumeAmmo(float Amount)
{
	if (!bInfiniteAmmo)
	{
		Ammo = FMath::Clamp(Ammo - Amount, 0.0f, MaxAmmo);
		OnChangeAmmo.Broadcast(Ammo);
		if (Ammo == 0.0f)
		{
			EndAttack();
		}
	}
}

void UBaseWeaponComponent::RestoreAmmo(float Amount)
{
	Ammo = FMath::Clamp(Ammo + Amount, 0.0f, MaxAmmo);
	OnChangeAmmo.Broadcast(Ammo);
}

void UBaseWeaponComponent::StartAttack()
{
	if (Ammo > 0.0f || bInfiniteAmmo)
	{
		IsAttacking = true;
	}
}
void UBaseWeaponComponent::EndAttack()
{
	IsAttacking = false;
}
bool UBaseWeaponComponent::IsAttack()
{
	return IsAttacking;
}

USceneComponent* UBaseWeaponComponent::GetDirectionComponent()
{
	return DirectionComponent;
}

void UBaseWeaponComponent::SetDirectionComponent(USceneComponent* Component)
{
	DirectionComponent = Component;
}

float UBaseWeaponComponent::GetCurrentAmmo()
{
	return Ammo;
}
float UBaseWeaponComponent::GetMaxAmmo()
{
	return MaxAmmo;
}