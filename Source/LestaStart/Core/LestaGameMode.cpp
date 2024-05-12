// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaGameMode.h"
#include "LestaCharacter.h"
#include "LestaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "LestaStart/Components/Attributes/LivingAttributeComponent.h"

void ALestaGameMode::StartPlay() 
{
	Super::StartPlay();	
}

void ALestaGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (IsValid(NewPlayer->GetPawn()))
	{
		NumberOfLivePlayers++;
		if (APawn* PlayerPawn = NewPlayer->GetPawn())
		{
			if (IsValid(PlayerPawn))
			{
				PlayerPawn->OnEndPlay.AddDynamic(this, &ALestaGameMode::OnDeathPlayer);
			}
		}		
	}
}

void ALestaGameMode::OnDeathPlayer(AActor* PlayerActor, EEndPlayReason::Type EndType)
{
	NumberOfLivePlayers--;
	// If all players death - restart level
	if(NumberOfLivePlayers <= 0)
	{
		NumberOfLivePlayers = 0;
		RestartLevel();
	}
}

void ALestaGameMode::RestartLevel()
{
	if (UWorld* World = GetWorld())
	{
		RestartLevelNotify(World); // Notify all client about restart level.
		// Restart
		World->ServerTravel("?Restart", false);
	}
}

void ALestaGameMode::RestartLevelNotify(UWorld* World)
{
	if (World)
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; Iterator++)
		{
			ALestaPlayerController* Controller = Cast<ALestaPlayerController>(Iterator->Get());
			if(IsValid(Controller))
			{
				Controller->Multicast_LevelRestart();
			}
		}
	}
}