// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeaponComponent.h"
#include "BlastWeaponComponent.generated.h"

class USceneComponent;
/**
 * Blast weapon - Make a blast in world - According chosed component location
 */
UCLASS()
class LESTASTART_API UBlastWeaponComponent : public UBaseWeaponComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
	float CurrentPower = 0.0f;
	/** An array of actors that will not take damage by this component */
	TArray<AActor*> IgnoredActors;

protected:
	/** Force generation rate per second, maximum power is 1.0 */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float PowerIPS;

public:
	UBlastWeaponComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Add an actor that will not take damage by this component */
	void AddIgnoreActor(AActor* Actor);
	/** Remove an actor that will not take damage by this component */
	void RemoveIgnoreActor(AActor* Actor);
};
