// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayersFinderComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFindDelegateOne, APawn*);
DECLARE_MULTICAST_DELEGATE(FFindDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LESTASTART_API UPlayersFinderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayersFinderComponent();

	/** Checks the player for proximity, line of sight to him (if enabled). */
	const TSet<APawn*>& SearchPlayers();

protected:
	virtual void BeginPlay() override;

	FTimerHandle TimerHandle;
	void OnTimerSearch();

public:
	FFindDelegateOne OnFoundPlayer;
	FFindDelegateOne OnFoundClosestPlayer;
	FFindDelegate OnNotFoundPlayers;

	UPROPERTY(EditAnywhere, Category = "Finder")
	bool bAutoSearch = false;

	UPROPERTY(EditAnywhere, Category = "Finder", meta = (ClampMin = "0.0", Units = "s"))
	float SearchInterval = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Finder", meta = (ClampMin = "0.0"))
	float SearchDistance = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Finder")
	bool bCheckDirectVisibility = true;

	UPROPERTY(EditAnywhere, Category = "Finder")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

private:
	TSet<APawn*> FoundPlayers;
};
