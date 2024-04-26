// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthTextComponent.h"
#include "Kismet/KismetMathLibrary.h"

UHealthTextComponent::UHealthTextComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthTextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bRotateToPlayer)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* LocalPlayerController = World->GetFirstPlayerController();
			if (LocalPlayerController)
			{
				APawn* LocalPlayerPawn = LocalPlayerController->GetPawn();
				if (IsValid(LocalPlayerPawn))
				{
					// Rotate Component to Player
					FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), LocalPlayerPawn->GetActorLocation());
					this->SetWorldRotation(Rotator);
				}
			}
		}
	}
}

void UHealthTextComponent::SetHealth(float Health)
{
	this->SetText(FText::AsNumber(FMath::Floor(Health)));
}