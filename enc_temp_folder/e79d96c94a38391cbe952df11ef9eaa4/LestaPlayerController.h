// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "LestaPlayerController.generated.h"

class UInputMappingContext;

/** Base Player Controller class for the Lesta Start project. */
UCLASS()
class LESTASTART_API ALestaPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PlayerUIClass;

	/** Input action assigned to change the observation target */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ChangeObservationAction;

	/** Added input mapping context on startup. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	/** Priority of InputMapping. */
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (ClampMin = 0))
	int32 InputMappingPriority = 0;

	/** Spectator mode */
	int32 ObservationPlayerIndex = 0;

	void OnChangeObservation(const FInputActionInstance& InputActionInstance);

	void OnDestroyPawn();

	virtual void BeginPlay() override;

};
