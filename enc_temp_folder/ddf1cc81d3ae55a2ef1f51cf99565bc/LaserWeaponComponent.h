// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeaponComponent.h"
#include "LaserWeaponComponent.generated.h"



class UArrowComponent;
class UNiagaraSystem;
class UNiagaraComponent;
/**
 * Laser Weapon - Create a laser
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LESTASTART_API ULaserWeaponComponent : public UBaseWeaponComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
	bool IsOverheat = false;

	FTimerHandle TimerOverheatHandle;

protected:
	// Problem - VFX effect appears delayed when moving character 
	UNiagaraComponent* LaserNiagaraComponent;

	float TraceSphereRadius = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0", Units = "s"))
	float DischargingSpeed = 1.0f; // Consume Ammo Per Second

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0", Units = "s"))
	float ChargingSpeed = 1.0f; // Restore Ammo Per Second
public:
	ULaserWeaponComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//** Sounds and effects *
	UPROPERTY(EditAnywhere, Category = "Weapon")
	UNiagaraSystem* LaserBeam;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Debug(FVector Location);
};