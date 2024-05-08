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

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (ClampMin = "0.0"))
	float RotationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Finder")
	UPlayersFinderComponent* FinderComponent;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

protected:
	/** Determines the direction and location of the shot */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USceneComponent* LaserDirectionComponent;

	/** Weapon component - implement weapon logic, cause damage */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	ULaserWeaponComponent* LaserComponent;

	virtual void OnFoundPlayer(APawn* FoundActor);
	virtual void OnNotFoundPlayers();

private:
	/** The rotation position of the turret to which it smoothly turns */
	UPROPERTY(Replicated)
	FRotator RotationTarget;

};
