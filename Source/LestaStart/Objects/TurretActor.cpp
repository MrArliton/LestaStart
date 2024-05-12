// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretActor.h"
#include "Net/UnrealNetwork.h"
#include "Components/ArrowComponent.h"
#include "LestaStart/Components/Weapons/LaserWeaponComponent.h"
#include "LestaStart/Components/Utility/PlayersFinderComponent.h"
#include "Kismet/KismetMathLibrary.h"

ATurretActor::ATurretActor() : ALivingMeshActor()
{
	// Replication
	bReplicates = true;
	NetUpdateFrequency = 10.0f;
	SetReplicateMovement(true);

	// Weapon
	LaserComponent = CreateDefaultSubobject<ULaserWeaponComponent>(TEXT("Laser Weapon"));
	LaserDirectionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LaserDirection"));
	LaserDirectionComponent->SetupAttachment(RootComponent);
	LaserComponent->SetDirectionComponent(LaserDirectionComponent);
	LaserComponent->SetIsReplicated(true);
	// Finder Component
	FinderComponent = CreateDefaultSubobject<UPlayersFinderComponent>("Player Finder Component");
	FinderComponent->OnFoundClosestPlayer.AddUObject(this, &ATurretActor::OnFoundPlayer);
	FinderComponent->OnNotFoundPlayers.AddUObject(this, &ATurretActor::OnNotFoundPlayers);
	FinderComponent->bAutoSearch = true;
	FinderComponent->SetSearchInterval(0.25f);

	RotationSpeed = 1.0f;
}

void ATurretActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurretActor, RotationTarget);
}

void ATurretActor::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	// Smooth turn
	FRotator ActorRotator = GetActorRotation();
	float Coef = FMath::Min(1.0f, RotationSpeed * DeltaTime);
	FRotator TargetRotator = FMath::Lerp(ActorRotator,RotationTarget,Coef);
	SetActorRotation(TargetRotator);
}

void ATurretActor::OnFoundPlayer(APawn* FoundActor)
{
	if (IsValid(LaserComponent))
	{
		if (IsValid(FoundActor))
		{
			RotationTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FoundActor->GetActorLocation());
			LaserComponent->StartAttack();
		}
		else
		{
			LaserComponent->EndAttack();
		}
	}
}

void ATurretActor::OnNotFoundPlayers()
{
	if (IsValid(LaserComponent))
	{
		LaserComponent->EndAttack();
	}
}