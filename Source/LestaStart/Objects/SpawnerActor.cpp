// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnerActor.h"

ASpawnerActor::ASpawnerActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComponent);
}

TArray<int32> ASpawnerActor::GetRandomArrayOfIndexes(int32 InMin, int32 InMax, int32 Size)
{
	if (InMin > InMax || InMax > Size)
	{
		UE_LOG(LogTemp, Error, TEXT("Error with Get Random Indexes, object: %s"), *GetName());
		return TArray<int32>{};
	}
	 
	int32 AmountOfPoints = FMath::RandRange(InMin, InMax);
	//** If we don't spawn objects */
	if (AmountOfPoints == 0) 
	{
		return TArray<int32>{};
	}
	//** Create array of indexes */
	TArray<int32> Indexes;
	for (int i = 0; i < Size; i++)
	{
		Indexes.Add(i);
	}	
	//** Remove some points to get {AmountOfPoints} points. */
	int32 AmountOfDeletePoints = Size - AmountOfPoints;
	for (int i = 0;i < AmountOfDeletePoints;i++)
	{ 
		Indexes.RemoveAt(FMath::RandRange(0, Indexes.Num()-1));
	}
	return Indexes;
}

void ASpawnerActor::BeginPlay()
{
	Super::BeginPlay();
	if (bOnBeginSpawn) 
	{
		this->Spawn();
	}	
}

void ASpawnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnerActor::Spawn()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn objects, world is undefined, object: %s"), *GetName());
		return;
	}

	if (bRandomSpawn)
	{
		TArray<int32> Indexes = GetRandomArrayOfIndexes(Min, Max, SpawnPoints.Num());

		for (int32 Index : Indexes)
		{
			if (UClass* SpawnClass = SpawnActorClass.Get())
			{
				FTransform SpawnTransform = GetActorTransform() + SpawnPoints[Index];
				World->SpawnActor(SpawnClass, &SpawnTransform);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot spawn objects, SpawnActorClass is undefined, object: %s"), *GetName());
				return;
			}
		}
	}
	else 
	{
		for (auto SpawnLocation : SpawnPoints)
		{
			if (UClass* SpawnClass = SpawnActorClass.Get())
			{
				World->SpawnActor(SpawnClass, &SpawnLocation);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot spawn objects, SpawnActorClass is undefined, object: %s"), *GetName());
				return;
			}
		}
	}
}