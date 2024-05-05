// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeaponComponent.h"
#include "LaserWeaponComponent.generated.h"



class UArrowComponent;
class UNiagaraSystem;
class UNiagaraComponent;
/**
 * Create a laser in world  
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LESTASTART_API ULaserWeaponComponent : public UBaseWeaponComponent
{
	GENERATED_BODY()

public:
	ULaserWeaponComponent();

	//** Sounds and effects *
	UPROPERTY(EditAnywhere, Category = "Weapon")
	UNiagaraSystem* LaserBeam;

	/** Debug visualization */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Debug(FVector Location);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/** Laser VFX */
	UPROPERTY();
	UNiagaraComponent* LaserNiagaraComponent;

	float TraceSphereRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0", Units = "s"))
	float DischargingSpeed; // Consume Ammo Per Second

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0", Units = "s"))
	float ChargingSpeed; // Restore Ammo Per Second

private:
	/** Attack logic and effects logic */
	void ActiveState(float DeltaTime);
	/** Don't attack, disable effects and wait  */
	void PendingState(float DeltaTime);

	/** Reloading flag - if out of ammo */
	UPROPERTY(Replicated)
	bool IsOverheat = false;
	/** Timer handle to turn off "IsOverheat" after reload time */
	FTimerHandle TimerOverheatHandle;
	/** Params for trace */
	FCollisionQueryParams TraceParams;

};