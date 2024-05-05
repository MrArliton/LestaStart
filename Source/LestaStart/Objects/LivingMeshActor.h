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
	ALivingMeshActor();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;
	/** Health attribute */
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Attribute")
	ULivingAttributeComponent* LivingAttributeComponent;
	/** Text to show health */
	UPROPERTY(EditDefaultsOnly, Category = "Utility")
	UHealthTextComponent* HealthTextComponent;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	virtual void OnDamagedAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	/** This is caused by a change in health */
	UFUNCTION()
	virtual void OnHealthChanged(float Health);
	/** Called when players health now is 0.0 */
	UFUNCTION()
	virtual void OnDeath();

};

