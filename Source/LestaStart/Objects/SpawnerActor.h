// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerActor.generated.h"

UCLASS()
class LESTASTART_API ASpawnerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnerActor();

	/** Activate spawn logic */
	void Spawn(bool RandomSpawn);
	/** If enabled, spawn a random number at random points of "SpawnActorClass" (according to the parameters)
	 * Otherwise, spawn "SpawnActorClass" at all points */
	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	bool bOnBeginSpawn = true;
	
	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (MakeEditWidget = true))
	TArray<FTransform> SpawnPoints;
	/** If enabled bOnBeginSpawn, activate random spawn logic */
	UPROPERTY(EditAnywhere, Category = "Spawner")
	bool bRandomSpawn = false;
	/** Min amount of actors spawn */
	UPROPERTY(EditAnywhere, Category = "Spawner|RandomSpawn", meta = (ClampMin = "0", EditCondition = "bRandomSpawn"))
	int32 Min = 1;
	/** Max amount of actors spawn */
	UPROPERTY(EditAnywhere, Category = "Spawner|RandomSpawn", meta = (ClampMin = "1", EditCondition = "bRandomSpawn"))
	int32 Max = 1;

protected:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneComponent;
	/** Class of actor that will be spawned */
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AActor> SpawnActorClass;

	virtual void BeginPlay() override;

private:
	/** Calculate a random array of unique indexes */
	TArray<int32> GetRandomArrayOfIndexes(int32 InMin, int32 InMax, int32 Size);
};
