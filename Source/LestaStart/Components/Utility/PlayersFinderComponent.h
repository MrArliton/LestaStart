// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayersFinderComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFindSignature, APawn*);
DECLARE_MULTICAST_DELEGATE(FOnNotFindSignature);


/**
 * The component that searches for players.
 * It only works on authority.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LESTASTART_API UPlayersFinderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayersFinderComponent();

	/** Checks the player for proximity, line of sight to him (if enabled bCheckDirectVisibility). */
	const TSet<APawn*>& SearchPlayers();

	/** If "SearchPlayers" have found the player call this event.
	 * Called several times if many players are found */
	FOnFindSignature OnFoundPlayer;
	/** If "SearchPlayers" have found the players, this event is triggered for the player actor closest to the owners */
	FOnFindSignature OnFoundClosestPlayer;
	/** If "SearchPlayers" did not find the players */
	FOnNotFindSignature OnNotFoundPlayers;

	/** Activates constant search, it cannot be disabled */
	UPROPERTY(EditAnywhere, Category = "Finder")
	bool bAutoSearch = false;

	/** Max distance to the players from the owner actor */
	UPROPERTY(EditAnywhere, Category = "Finder", meta = (ClampMin = "0.0"))
	float SearchDistance;
	/** If enabled, check using trace with "CollisionChannel" whether an actor is directly visible */
	UPROPERTY(EditAnywhere, Category = "Finder")
	bool bCheckDirectVisibility = true;
	/** Add owner actor to trace ignore list */
	UPROPERTY(EditAnywhere, Category = "Finder", meta = (EditCondition = "bCheckDirectVisibility"))
	bool bTraceIgnoreOwner = true;
	/** CollisionChannel for tracing when checking the direct view of an actor */
	UPROPERTY(EditAnywhere, Category = "Finder", meta = (EditCondition = "bCheckDirectVisibility"))
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	void SetSearchInterval(float NewSearchInterval);

	float GetSearchInterval() const;

protected:
	/** The interval between calls to "SearchPlayers" function */
	UPROPERTY(EditAnywhere, Category = "Finder", meta = (ClampMin = "0.0", Units = "s"))
	float SearchInterval;

	FCollisionQueryParams TraceParams;

	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle;
	void OnTimerSearch();

	/** Last found players */
	TSet<APawn*> FoundPlayers;
};
