// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

void ALestaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() >= ENetMode::NM_ListenServer)
	{
		if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
		{
			if (auto* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSystem->AddMappingContext(InputMapping, InputMappingPriority);
			}
		}
		// Setup Inputs
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		if (EIC)
		{
			EIC->BindAction(ChangeObservationAction, ETriggerEvent::Triggered, this, &ALestaPlayerController::OnChangeObservation);
		}
		else
		{
			UE_LOG(LogInput, Error, TEXT("Unexpected input component class: %s"), *GetFullNameSafe(InputComponent))
		}
	}
}


void ALestaPlayerController::Server_OnChangeObservation_Implementation()
{
	if (GetStateName() == NAME_Inactive)
	{
		// Try to get next player
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator() + ObservationPlayerIndex + 1; Iterator; ++Iterator)
		{
			if(ChangeObservationPlayer(Iterator->Get(), Iterator.GetIndex()))
			{
				// If change is successful 
				return;
			}
		}
		// Try to get from the entire list
		for(FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ChangeObservationPlayer(Iterator->Get(), Iterator.GetIndex()))
			{
				// If change is successful
				return;
			}
		}
	}
}

/** If changing successful - return true */
bool ALestaPlayerController::ChangeObservationPlayer(APlayerController* PlayerController, int32 Index)
{
	if (IsValid(PlayerController))
	{
		// Check PlayerPawn
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (IsValid(PlayerPawn))
		{
			ObservationPlayerIndex = Index;
			SetViewTargetWithBlend(PlayerPawn, 0.5f);
			return true;
		}
	}
	/** Controller is invalid or does not exist pawn */
	return false;
}

void ALestaPlayerController::OnChangeObservation(const FInputActionInstance& InputActionInstance)
{
	// Player cannot play -> allow change observation  
	if (GetStateName() == NAME_Inactive)
	{
		Server_OnChangeObservation();
	}
}

