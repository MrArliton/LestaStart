// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

/**
 * Displays character parameters (health, ammo, ...)
 */
UCLASS()
class LESTASTART_API UPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	float HealthPercentage = 1.0f;
	float AmmoPercantage = 1.0f;

	float MaxHealth = 10.0f;
	float MaxAmmo = 10.0f;
	
	TMap<int32, FText> WeaponTexts;
	int32 WeaponId = 0;
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintPure)
	float GetHealthPercentage();
	UFUNCTION(BlueprintPure)
	float GetAmmoPercentage();
	UFUNCTION(BlueprintPure)
	FText GetWeaponText();
public:
	UFUNCTION(BlueprintCallable, Category = "UpdateUI")
	void OnHealthChanged(float Health);

	UFUNCTION(BlueprintCallable, Category = "UpdateUI")
	void OnAmmoChanged(float Ammo);

	UFUNCTION(BlueprintCallable, Category = "UpdateUI")
	void OnWeaponChanged(int32 WeaponIndex);

	UFUNCTION(BlueprintCallable, Category = "UpdateUI")
	void AddWeapon(int32 Id, FText Name);

	UFUNCTION(BlueprintCallable, Category = "UpdateUI")
	void SetMaxAmmo(float Ammo);

	UFUNCTION(BlueprintCallable, Category = "UpdateUI")
	void SetMaxHealth(float Health);



};
