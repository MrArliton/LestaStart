// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUIWidget.h"

void UPlayerUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

float UPlayerUIWidget::GetHealthPercentage()
{
	return HealthPercentage;
}


float UPlayerUIWidget::GetAmmoPercentage()
{
	return AmmoPercantage;
}

FText UPlayerUIWidget::GetWeaponText()
{
	if (WeaponTexts.Contains(WeaponId))
	{
		return WeaponTexts[WeaponId];
	}
	return FText::GetEmpty();
}

void UPlayerUIWidget::OnHealthChanged(float Health)
{
	HealthPercentage = Health / MaxHealth;
}

void UPlayerUIWidget::OnAmmoChanged(float Ammo)
{
	AmmoPercantage = Ammo / MaxAmmo;
}

void UPlayerUIWidget::OnWeaponChanged(int32 WeaponIndex)
{
	WeaponId = WeaponIndex;
}

void UPlayerUIWidget::AddWeapon(int32 Id, FText Name)
{
	WeaponTexts.Add(Id, Name);
}

void UPlayerUIWidget::SetMaxAmmo(float Ammo)
{
	MaxAmmo = Ammo;
}

void UPlayerUIWidget::SetMaxHealth(float Health)
{
	MaxHealth = Health;
}