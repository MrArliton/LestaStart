// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LestaGameMode.generated.h"

class ULestaHUD;

/** Base Game Mode class for the Lesta Start project. */
UCLASS()
class LESTASTART_API ALestaGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	int32 NumberOfLivePlayers = 0;

public:
	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer);
	UFUNCTION()
	virtual void OnDeathPlayer();

	void RestartLevel();	
};
