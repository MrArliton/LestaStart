// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretActor.h"
#include "Components/ArrowComponent.h"
#include "LestaStart/Components/Weapons/LaserWeaponComponent.h"
#include "LestaStart/Components/Utility/PlayersFinderComponent.h"
#include "Kismet/KismetMathLibrary.h"

ATurretActor::ATurretActor() : ALivingMeshActor()
{
	// Weapon
	LaserComponent = CreateDefaultSubobject<ULaserWeaponComponent>(TEXT("Laser Weapon"));
	LaserDirectionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LaserDirection"));
	LaserDirectionComponent->SetupAttachment(RootComponent);
	LaserComponent->SetDirectionComponent(LaserDirectionComponent);
	
	// Finder Component
	FinderComponent = CreateDefaultSubobject<UPlayersFinderComponent>("Player Finder Component");
	FinderComponent->OnFoundPlayer.AddUObject(this, &ATurretActor::OnFoundPlayer);
	FinderComponent->OnLosePlayer.AddUObject(this, &ATurretActor::OnLosePlayer);
}

void ATurretActor::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	// Smooth turn
	FRotator ActorRotator = GetActorRotation();
	float Coef = FMath::Min(1.0f, RotationSpeed * DeltaTime);
	SetActorRotation(ActorRotator - (ActorRotator - RotationToPlayer) * Coef);
}

void ATurretActor::OnFoundPlayer(AActor* FoundActor)
{
	if (IsValid(FoundActor))
	{
		RotationToPlayer = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FoundActor->GetActorLocation());
		LaserComponent->StartAttack();
	}
	else
	{
		LaserComponent->EndAttack();
	}
}

void ATurretActor::OnLosePlayer(AActor* LostActor)
{
	LaserComponent->EndAttack();
}