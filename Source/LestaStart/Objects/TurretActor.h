// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LivingMeshActor.h"
#include "TurretActor.generated.h"

class UPlayersFinderComponent;
class ULaserWeaponComponent;
class UArrowComponent;

/**
 * Living Mesh Actor - Laser Turret 
 */
UCLASS()
class LESTASTART_API ATurretActor : public ALivingMeshActor
{
	GENERATED_BODY()


public:
	ATurretActor();

protected:
	virtual void Tick(float DeltaTime) override;

	void OnFoundPlayer(AActor* FoundActor);
	void OnLosePlayer(AActor* LostActor);

private:
	FRotator RotationToPlayer;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USceneComponent* LaserDirectionComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	ULaserWeaponComponent* LaserComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (ClampMin = "0.0"))
	float RotationSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Finder")
	UPlayersFinderComponent* FinderComponent;
};
