// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LivingMeshActor.generated.h"

class UStaticMeshComponent;
class ULivingAttributeComponent;
class UHealthTextComponent;

UCLASS()
class LESTASTART_API ALivingMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALivingMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnDamagedAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION()
	virtual void OnHealthChanged(float Health);
	UFUNCTION()
	virtual void OnDeath();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Attribute")
	ULivingAttributeComponent* LivingAttributeComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Utility")
	UHealthTextComponent* HealthTextComponent;


};

