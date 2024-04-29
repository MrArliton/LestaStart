// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LestaStart/Objects/Weapons/BaseWeapon.h"
#include "InputAction.h"
#include "BlastWeapon.generated.h"

class UBlastWeaponComponent;

/**
 * Blast weapon actor contains Blast Weapon Component for attacking.
 * Can be attached to another actor
 */
UCLASS()
class LESTASTART_API ABlastWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	ABlastWeapon();

	virtual void AttachWeapon(ALestaCharacter* Character, FName SocketName) override;

	virtual void SetupInputComponent(UInputComponent* NewInputComponent) override;

	virtual void Deactivate(bool IsVisible = true) override;
	virtual bool IsAttacking() override;

protected:

	virtual void PreDetachWeapon() override;

	UPROPERTY(EditAnywhere, Category = "Weapon|Input")
	TObjectPtr<UInputAction> AttackInputAction;

	virtual void OnStartAttack(const FInputActionInstance& InputActionInstance);
	virtual void OnEndAttack(const FInputActionInstance& InputActionInstance);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UBlastWeaponComponent* BlastWeaponComponent;
};
