// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "HealthTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class LESTASTART_API UHealthTextComponent : public UTextRenderComponent
{
	GENERATED_BODY()
	
public:
	UHealthTextComponent();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere)
	bool bRotateToPlayer = true;

	void SetHealth(float Health);
};
