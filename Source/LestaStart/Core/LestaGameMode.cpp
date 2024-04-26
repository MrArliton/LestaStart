// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaGameMode.h"
#include "LestaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/Attributes/LivingAttributeComponent.h"

void ALestaGameMode::StartPlay() 
{
	Super::StartPlay();

	if (UWorld* World = GetWorld()) 
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(IsValid(PlayerController))
		{
			APawn* Pawn = PlayerController->GetPawn();
			if (ALestaCharacter* LestaCharacter = Cast<ALestaCharacter>(Pawn)) 
			{
				ULivingAttributeComponent* LivingAttribute = LestaCharacter->GetLivingAttributeComponent();
				if (IsValid(LivingAttribute))
				{
					LivingAttribute->OnDeath.AddDynamic(this, &ALestaGameMode::RestartLevel);
				}
			}
		}
	}

}

void ALestaGameMode::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}