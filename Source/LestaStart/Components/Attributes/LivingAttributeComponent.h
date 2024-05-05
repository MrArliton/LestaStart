
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivingAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthDelegateOne, float, Health);
/**
 * Represents the life attribute of an actor
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LESTASTART_API ULivingAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULivingAttributeComponent();

	/** The event is triggered when health = 0.0 */
	UPROPERTY(BlueprintAssignable, Category = "Attribute")
	FDeathDelegate OnDeath;
	/** The event is triggered whenever health changes. */
	UPROPERTY(BlueprintAssignable, Category = "Attribute")
	FHealthDelegateOne OnChangeHealth;

private:
	/** Special flag is enabled if the OnDeath event was raised
	 * If health becomes >0.0, the flag will be disabled */
	bool IsDeath = false;

	UPROPERTY(EditAnywhere, Category = "Attribute", meta = (ClampMin = "0.1"))
	float MaxHealth = 100.0f;

protected:
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Health)
	float Health;

	virtual void BeginPlay() override;
public:	

	/** Add to health -> Value */
	void Heal(float Value);
	/** Subtract from health -> Value */
	void Damage(float Value);
	/** if health == MaxHealth, return true */
	bool IsFullHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attribute")
	float GetMaxHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attribute")
	float GetCurrentHealth() const;

	UFUNCTION()
	void OnRep_Health();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
