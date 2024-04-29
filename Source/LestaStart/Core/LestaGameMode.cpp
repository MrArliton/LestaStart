// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaGameMode.h"
#include "LestaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LestaStart/Components/Attributes/LivingAttributeComponent.h"

void ALestaGameMode::StartPlay() 
{
	Super::StartPlay();
	
}

void ALestaGameMode::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}