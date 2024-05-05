// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

void ALestaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (auto* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->AddMappingContext(InputMapping, InputMappingPriority);
		}		
	}
	// Setup Inputs
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if(EIC)
	{
		EIC->BindAction(ChangeObservationAction, ETriggerEvent::Triggered, this, &ALestaPlayerController::OnChangeObservation);
	}
	else
	{
		UE_LOG(LogInput, Error, TEXT("Unexpected input component class: %s"), *GetFullNameSafe(InputComponent))
	}
}

void ALestaPlayerController::OnChangeObservation(const FInputActionInstance& InputActionInstance)
{
	UE_LOG(LogTemp, Warning,TEXT("%s"), *GetStateName().ToString());
	if (GetStateName() == NAME_Spectating)
	{
		if (HasAuthority())
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator() + ObservationPlayerIndex + 1; Iterator; ++Iterator)
			{
				// Check PlayerController
				APlayerController* PlayerController = Iterator->Get();
				if (IsValid(PlayerController))
				{
					// Check PlayerPawn
					APawn* PlayerPawn = PlayerController->GetPawn();
					if(IsValid(PlayerPawn))
					{
						ObservationPlayerIndex = Iterator.GetIndex();
						SetViewTargetWithBlend(PlayerPawn, 0.5f);
					}
				}
			}
		}
	}
}