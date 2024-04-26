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

protected:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AActor> SpawnActorClass;
			
	TArray<int32> GetRandomArrayOfIndexes(int32 InMin, int32 InMax, int32 Size);

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Spawn();
	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	bool bOnBeginSpawn = true;

	UPROPERTY(EditAnywhere, Category = "Spawner", Meta = (MakeEditWidget = true))
	TArray<FTransform> SpawnPoints;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	bool bRandomSpawn = false;

	UPROPERTY(EditAnywhere, Category = "Spawner|RandomSpawn", meta = (ClampMin = "0", EditCondition = "bRandomSpawn"))
	int32 Min = 1;

	UPROPERTY(EditAnywhere, Category = "Spawner|RandomSpawn", meta = (ClampMin = "1", EditCondition = "bRandomSpawn"))
	int32 Max = 1;
};
