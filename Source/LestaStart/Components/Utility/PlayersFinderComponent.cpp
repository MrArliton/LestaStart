

#include "PlayersFinderComponent.h"
#include "Camera/CameraComponent.h"

UPlayersFinderComponent::UPlayersFinderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayersFinderComponent::BeginPlay()
{
	Super::BeginPlay();

	// Add ignore owner
	if (bTraceIgnoreOwner)
	{
		AActor* Owner = GetOwner();
		if (IsValid(Owner))
		{
			TraceParams.AddIgnoredActor(Owner);
		}
	}

	// Set timer
	if (GetOwnerRole() == ROLE_Authority)
	{
		UWorld* World = GetWorld();

		if (World && bAutoSearch)
		{
			World->GetTimerManager().SetTimer(TimerHandle, this, &UPlayersFinderComponent::OnTimerSearch, SearchInterval, true);
		}
	}
}
// Timer function
void UPlayersFinderComponent::OnTimerSearch()
{
	SearchPlayers();
}

const TSet<APawn*>& UPlayersFinderComponent::SearchPlayers()
{
	// Preparing
	FoundPlayers.Empty();
	UWorld* World = GetWorld();
	if (!World)
	{
		return FoundPlayers;
	}

	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return FoundPlayers;
	}

	const FVector Start = OwnerActor->GetActorLocation();
	// For searching closest player 
	APawn* ClosestPlayer = nullptr;
	float ClosestPlayerDistance = TNumericLimits<float>::Max();
	// Search players
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = (*Iterator).Get();

		if (IsValid(PlayerController))
		{
			APawn* PlayerPawn = PlayerController->GetPawn();			
			if (IsValid(PlayerPawn))
			{
				const FVector End = PlayerPawn->GetActorLocation();
				float DistanceToPlayer = (End - Start).Length();
				/** Check this pawn in the search area */
				if (DistanceToPlayer < SearchDistance)
				{
					if (!bCheckDirectVisibility) // No check trace
					{
						FoundPlayers.Add(PlayerPawn);
						OnFoundPlayer.Broadcast(PlayerPawn);
						// For search closest player 
						if (DistanceToPlayer < ClosestPlayerDistance)
						{
							ClosestPlayer = PlayerPawn;
						}
					}
					else // Check trace
					{
						FHitResult HitResult;
						// Trace from this owner actor to player 
						GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, CollisionChannel.GetValue(), TraceParams);

						AActor* HitActor = HitResult.GetActor();
						if (IsValid(HitActor) && (Cast<AActor>(PlayerPawn) == HitActor))
						{
							FoundPlayers.Add(PlayerPawn);
							OnFoundPlayer.Broadcast(PlayerPawn);
							// For search closest player 
							if (DistanceToPlayer < ClosestPlayerDistance)
							{
								ClosestPlayer = PlayerPawn;
							}
						}
					}
				}
			}
		}
	}	

	if (ClosestPlayer)
	{
		OnFoundClosestPlayer.Broadcast(ClosestPlayer);
	}

	if (FoundPlayers.IsEmpty()) 
	{
		OnNotFoundPlayers.Broadcast();
	}

	return FoundPlayers;
}