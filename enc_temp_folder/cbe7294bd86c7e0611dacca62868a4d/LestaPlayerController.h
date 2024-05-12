// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "LestaPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLestaControllerDelegate);

class UInputMappingContext;

/** Base Player Controller class for the Lesta Start project. */
UCLASS()
class LESTASTART_API ALestaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Control")
	FLestaControllerDelegate OnRestartLevel;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LevelRestart();

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
	/** Index of observed player*/
	int32 ObservationPlayerIndex = 0;

	/** Change observer player to next */
	UFUNCTION(Server, Unreliable)
	void Server_OnChangeObservation();

	/** Input action event - Call "Server_OnChangeObservation"  */
	void OnChangeObservation(const FInputActionInstance& InputActionInstance);

	/** If changing succesfull - return true
	 * @Param Index - index of "PlayerController" in player controllers list  */
	bool ChangeObservationPlayer(APlayerController* PlayerController, int32 Index);

	virtual void BeginPlay() override;

};
