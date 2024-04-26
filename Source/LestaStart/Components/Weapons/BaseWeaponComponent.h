// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseWeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponDelegateOne, float, Ammo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LESTASTART_API UBaseWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBaseWeaponComponent();

	USceneComponent* DirectionComponent;
	USceneComponent* GetDirectionComponent();
	void SetDirectionComponent(USceneComponent* Component);

protected:
	virtual void BeginPlay() override;

	float Ammo;	
	void ConsumeAmmo(float Amount);
	void RestoreAmmo(float Amount);

	bool IsAttacking = false;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void StartAttack();
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "BaseWeapon")
	virtual bool IsAttack();

	UPROPERTY(BlueprintAssignable, Category = "BaseWeapon")
	FWeaponDelegateOne OnChangeAmmo;

	UFUNCTION(BlueprintCallable, Category = "BaseWeapon")
	float GetCurrentAmmo();
	UFUNCTION(BlueprintCallable, Category = "BaseWeapon")
	float GetMaxAmmo();

protected:
	UPROPERTY(EditAnywhere, Category = "BaseWeapon")
	bool bInfiniteAmmo = false;

	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0"))
	float BaseDamage = 2.0f;

	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0"))
	float AttackDistance = 100.0f; 

	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0"))
	float MaxAmmo = 2.0f; 

	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0", Units = "s"))
	float ReloadTime = 1.0f;
};
