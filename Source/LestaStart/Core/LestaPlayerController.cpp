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
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator() + ObservationPlayerIndex + 1; Iterator; ++Iterator)
		{
			// Check PlayerController from ObservationPlayerIndex.
			APlayerController* PlayerController = Iterator->Get();
			if (IsValid(PlayerController))
			{
				// Check PlayerPawn
				APawn* PlayerPawn = PlayerController->GetPawn();
				if (IsValid(PlayerPawn))
				{
					ObservationPlayerIndex = Iterator.GetIndex();
					SetViewTargetWithBlend(PlayerPawn, 0.5f);
					return;
				}
			}
		}
		// Check from start of players list. 
		for(FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			// If we have reached the previous controller, then the rest have already been checked. So we stop iterating.
			if(Iterator.GetIndex() == ObservationPlayerIndex)
			{
				return;
			}

			// Check PlayerController
			APlayerController* PlayerController = Iterator->Get();
			if (IsValid(PlayerController))
			{
				// Check PlayerPawn
				APawn* PlayerPawn = PlayerController->GetPawn();
				if (IsValid(PlayerPawn))
				{
					ObservationPlayerIndex = Iterator.GetIndex();
					SetViewTargetWithBlend(PlayerPawn, 0.5f);
					return;
				}
			}
		}
	}
}

void ALestaPlayerController::OnChangeObservation(const FInputActionInstance& InputActionInstance)
{
	// Inactive state - Player Controler don't have a pawn 
	if (GetStateName() == NAME_Inactive)
	{
		Server_OnChangeObservation();
	}
}