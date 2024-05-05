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

	/** Event triggered every time "Ammo" is changed */
	UPROPERTY(BlueprintAssignable, Category = "BaseWeapon")
	FWeaponDelegateOne OnChangeAmmo;

	UPROPERTY(EditAnywhere, Category = "BaseWeapon")
	bool bInfiniteAmmo = false;
	
	/** Determines the damage of the weapon */
	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0"))
	float AttackDistance;
	
	/** Max value of "Ammo" */
	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0"))
	float MaxAmmo;
	
	/** Time until the recharging process is completed  */
	UPROPERTY(EditAnywhere, Category = "BaseWeapon", meta = (ClampMin = "0.0", Units = "s"))
	float ReloadTime;
	
	/** Is attacking now ?  */
	UFUNCTION(BlueprintCallable, Category = "BaseWeapon")
	virtual bool IsAttack() const;

	/** Call RPC to server for starting attack 
	* @param StartLocaly - start attack locally for showing effects  */
	virtual void StartAttack(bool StartLocaly = true);
	/** Call RPC to server for end attack
	* @param EndLocaly - end attack locally for showing effects  */
	virtual void EndAttack(bool EndLocaly = true);
	/** DirectionComponent determines the direction and location of the shot */
	void SetDirectionComponent(USceneComponent* Component);

	UFUNCTION(BlueprintCallable, Category = "BaseWeapon")
	float GetCurrentAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "BaseWeapon")
	float GetMaxAmmo() const;

	/** DirectionComponent determines the direction and location of the shot */
	USceneComponent* GetDirectionComponent() const;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION(Server, Unreliable)
	virtual void Server_StartAttack();

	UFUNCTION(Server, Unreliable)
	virtual void Server_EndAttack();

	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Ammo)
	float Ammo;

	void ConsumeAmmo(float Amount);
	void RestoreAmmo(float Amount);

	UPROPERTY(Replicated)
	bool IsAttacking = false;
	/** Determines the direction and location of the shot */
	USceneComponent* DirectionComponent;

};
