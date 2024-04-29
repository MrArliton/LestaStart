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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Ammo)
	float Ammo;	
	
	void ConsumeAmmo(float Amount);
	void RestoreAmmo(float Amount);

	/** React on change IsAttacking */
	UFUNCTION()
	virtual void OnRep_IsAttacking();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsAttacking)
	bool IsAttacking = false;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "BaseWeapon")
	virtual bool IsAttack();

	/** Call RPC to server for starting attack 
	* @param StartLocaly - start attack locally for showing effects  */
	virtual void StartAttack(bool StartLocaly = true);
	/** Call RPC to server for end attack
	* @param EndLocaly - end attack locally for showing effects  */
	virtual void EndAttack(bool EndLocaly = true);

	UFUNCTION(Server, Unreliable)
	virtual void Server_StartAttack();

	UFUNCTION(Server, Unreliable)
	virtual void Server_EndAttack();

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
