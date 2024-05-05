// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUIWidget.h"

void UPlayerUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

float UPlayerUIWidget::GetHealthPercentage() const
{
	return HealthPercentage;
}


float UPlayerUIWidget::GetAmmoPercentage() const
{
	return AmmoPercantage;
}

float UPlayerUIWidget::GetChargePercentage() const
{
	return ChargePercantage;
}

FText UPlayerUIWidget::GetWeaponText() const
{
	if (WeaponTexts.Contains(WeaponId))
	{
		return WeaponTexts[WeaponId];
	}
	return FText::GetEmpty();
}

void UPlayerUIWidget::OnHealthChanged(float Health)
{
	HealthPercentage = FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f);
}

void UPlayerUIWidget::OnAmmoChanged(float Ammo)
{
	AmmoPercantage = FMath::Clamp(Ammo / MaxAmmo, 0.0f, 1.0f);
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

void UPlayerUIWidget::OnChargeChanged(float Charge)
{
	ChargePercantage = FMath::Clamp(Charge, 0.0f, 1.0f);
}