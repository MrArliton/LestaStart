// Fill out your copyright notice in the Description page of Project Settings.

#include "LivingMeshActor.h"
#include "../Components/Utility/HealthTextComponent.h"
#include "../Components/Attributes/LivingAttributeComponent.h"

ALivingMeshActor::ALivingMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	// Object is damageable
	this->SetCanBeDamaged(true);

	// Main Component - Scene components
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	// Static Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SceneComponent);

	// Health Text Component
	HealthTextComponent = CreateDefaultSubobject<UHealthTextComponent>(TEXT("Health Text"));
	HealthTextComponent->SetupAttachment(StaticMesh);
	HealthTextComponent->SetText(FText::FromString(TEXT("Health")));
	// Living Attribute Component - Functional components
	LivingAttributeComponent = CreateDefaultSubobject<ULivingAttributeComponent>(TEXT("LivingAttribute"));

	// Events 
	LivingAttributeComponent->OnDeath.AddDynamic(this, &ALivingMeshActor::OnDeath);
	LivingAttributeComponent->OnChangeHealth.AddDynamic(this, &ALivingMeshActor::OnHealthChanged);
	this->OnTakeAnyDamage.AddDynamic(this, &ALivingMeshActor::OnDamagedAny);
}

// Called when the game starts or when spawned
void ALivingMeshActor::BeginPlay()
{
	Super::BeginPlay();
	HealthTextComponent->SetHealth(LivingAttributeComponent->GetCurrentHealth());
}

void ALivingMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

