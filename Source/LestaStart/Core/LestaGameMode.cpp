// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaGameMode.h"
#include "LestaCharacter.h"
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
		if (ALestaCharacter* Character = Cast<ALestaCharacter>(NewPlayer->GetPawn()))
		{
			ULivingAttributeComponent* LivingComponent = Character->GetLivingAttributeComponent();
			if (IsValid(LivingComponent))
			{
				LivingComponent->OnDeath.AddDynamic(this, &ALestaGameMode::OnDeathPlayer);
			}
		}
	}
}

void ALestaGameMode::OnDeathPlayer()
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
	GetWorld()->ServerTravel("?Restart", false);
}