// Fill out your copyright notice in the Description page of Project Settings.

#include "LivingMeshActor.h"
#include "Net/UnrealNetwork.h"
#include "../Components/Utility/HealthTextComponent.h"
#include "../Components/Attributes/LivingAttributeComponent.h"

ALivingMeshActor::ALivingMeshActor()
{
	/** Replication */
	bReplicates = true;
	SetReplicateMovement(true);
	NetUpdateFrequency = 20.f;

	PrimaryActorTick.bCanEverTick = true;
	/** Object is damageable */
	SetCanBeDamaged(true);

	/** Not Replicated Static Mesh */
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	if (StaticMesh)
	{
		SetRootComponent(StaticMesh);
	}

	/** Replicated Living Attribute Component - Functional components */
	LivingAttributeComponent = CreateDefaultSubobject<ULivingAttributeComponent>(TEXT("LivingAttribute"));
	if (LivingAttributeComponent)
	{
		LivingAttributeComponent->SetIsReplicated(true);
		LivingAttributeComponent->OnDeath.AddDynamic(this, &ALivingMeshActor::OnDeath);
		LivingAttributeComponent->OnChangeHealth.AddDynamic(this, &ALivingMeshActor::OnHealthChanged);
	}
	/** Not Replicated Health Text Component */
	HealthTextComponent = CreateDefaultSubobject<UHealthTextComponent>(TEXT("Health Text"));
	if (HealthTextComponent)
	{
		HealthTextComponent->SetupAttachment(StaticMesh);
		HealthTextComponent->SetText(FText::FromString(TEXT("Health")));
	}

	this->OnTakeAnyDamage.AddDynamic(this, &ALivingMeshActor::OnDamagedAny);
}

void ALivingMeshActor::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(HealthTextComponent))
	{
		HealthTextComponent->SetHealth(LivingAttributeComponent->GetCurrentHealth());
	}
}

void ALivingMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALivingMeshActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALivingMeshActor, LivingAttributeComponent);
}

void ALivingMeshActor::OnDamagedAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	LivingAttributeComponent->Damage(Damage);
}

void ALivingMeshActor::OnHealthChanged(float Health)
{
	if (IsValid(HealthTextComponent))
	{
		HealthTextComponent->SetHealth(Health);
	}
}


void ALivingMeshActor::OnDeath()
{
	Destroy();
}

