// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LestaStart/Objects/Weapons/BaseWeapon.h"
#include "InputAction.h"
#include "LaserWeapon.generated.h"

class ULaserWeaponComponent;
/**
 * Laser weapon actor contains Laser Weapon Component for attacking. 
 * Can be attached to another actor
 */
UCLASS()
class LESTASTART_API ALaserWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	ALaserWeapon();

	virtual void SetupInputComponent(UInputComponent* NewInputComponent) override;

	virtual void Deactivate(bool IsVisible = true) override;

	virtual bool IsAttacking() override;

protected:
	/** Input action for start attack */
	UPROPERTY(EditAnywhere, Category = "Weapon|Input")
	TObjectPtr<UInputAction> AttackInputAction;

	/** Functions that bind to an input component with AttackInputAction */
	virtual void OnStartAttack(const FInputActionInstance& InputActionInstance);
	virtual void OnEndAttack(const FInputActionInstance& InputActionInstance);

	/** Determines the direction and location of the shot */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	USceneComponent* LaserDirectionComponent;

	/** Weapon component - implement weapon logic, cause damage */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	ULaserWeaponComponent* LaserWeaponComponent;
};
