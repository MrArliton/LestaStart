// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthDelegateOne, float, Health);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LESTASTART_API ULivingAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULivingAttributeComponent();

	UPROPERTY(BlueprintAssignable, Category = "Attribute")
	FHealthDelegate OnDeath;
	UPROPERTY(BlueprintAssignable, Category = "Attribute")
	FHealthDelegateOne OnChangeHealth;

private:
	bool IsDeath = false;

	UPROPERTY(EditAnywhere, Category = "Attribute", meta = (ClampMin = "0.1"))
	float MaxHealth = 100.0f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Health)
	float Health;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Heal(float Value);
	void Damage(float Value);

	bool IsFullHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attribute")
	float GetMaxHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attribute")
	float GetCurrentHealth() const;

	UFUNCTION()
	void OnRep_Health();

};
