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

	virtual void AttachWeapon(ALestaCharacter* Character, FName SocketName) override;

	virtual bool IsAttacking() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon|Input")
	TObjectPtr<UInputAction> AttackInputAction;

	virtual void OnStartAttack(const FInputActionInstance& InputActionInstance);
	virtual void OnEndAttack(const FInputActionInstance& InputActionInstance);


	UPROPERTY(EditAnywhere, Category = "Weapon")
	USceneComponent* LaserDirectionComponent;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	ULaserWeaponComponent* LaserWeaponComponent;
	
	/** The sound of the laser overheat(reloading) */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound")
	UAudioComponent* OverheatSoundComponent;
	/** The sound of the laser */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound")
	UAudioComponent* LaserSoundComponent;
	/** The sound of a laser hitting an object */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound")
	UAudioComponent* SparkSoundComponent;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ULaserWeaponComponent* GetLaserWeaponComponent();

};
