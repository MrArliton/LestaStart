// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayersFinderComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UPlayersFinderComponent::UPlayersFinderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlayersFinderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UPlayersFinderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// For one local player --- In the future for many players ?
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	AActor* OwnerActor = GetOwner();
	if (IsValid(PlayerPawn) && IsValid(OwnerActor))
	{
		const FVector Start = OwnerActor->GetActorLocation();
		const FVector End = PlayerPawn->GetActorLocation();
		// Calculate Distance between Player and OwnerActor 
		if ((End - Start).Length() < SearchDistance)
		{
			FHitResult HitResult;
			// Trace from this OwnerActor to Player    
			GetWorld()->LineTraceSingleByChannel(HitResult, OwnerActor->GetActorLocation(), PlayerPawn->GetActorLocation(), ECC_Pawn);

			AActor* HitActor = HitResult.GetActor();
			// Only Actors have Camera ? (At least in our game)
			UCameraComponent* CameraComponent;
			if(IsValid(HitActor) && IsValid(CameraComponent = HitActor->GetComponentByClass<UCameraComponent>()))
			{				  
				OnFoundPlayer.Broadcast(HitActor);
				IsFound = true;	
			}
			else if(IsFound)
			{
				OnLosePlayer.Broadcast(HitActor);
				IsFound = false;
			}
		}
		else if(IsFound)
		{
			OnLosePlayer.Broadcast(PlayerPawn);
			IsFound = false;
		}

	}
}

