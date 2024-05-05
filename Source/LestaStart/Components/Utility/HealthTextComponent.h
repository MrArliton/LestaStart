// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "HealthTextComponent.generated.h"

/**
 * Displaying lives as text in the world
 */
UCLASS()
class LESTASTART_API UHealthTextComponent : public UTextRenderComponent
{
	GENERATED_BODY()
	
public:
	UHealthTextComponent();

	/** Rotate text to local player */
	UPROPERTY(EditAnywhere, Category = "HealthText")
	bool bRotateToPlayer = true;

	/** Set the displayed health */
	void SetHealth(float Health);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Tick interval - TickComponent call RotateToPlayer function */
	UPROPERTY(EditDefaultsOnly, Category = "HealthText")
	float RotationUpdateInterval;
	
private:
	/** Rotate component to local player */
	void RotateToPlayer();

};
