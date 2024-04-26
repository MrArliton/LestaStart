// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LestaStart/Objects/Weapons/BaseWeapon.h"
#include "InputAction.h"
#include "LestaCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDelegateOne, ABaseWeapon*, NewWeapon);

class UCameraComponent;
class ULivingAttributeComponent;

UCLASS()
class LESTASTART_API ALestaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALestaCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FCharacterDelegateOne OnChangeWeapon;
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	/** Input action assigned to movement. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveInputAction;

	/** Input action assigned to camera movement. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookInputAction;
	/** Input action assigned to change weapon. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ChangeWeaponInputAction;
	/** Input action assigned to change weapon by numbers. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ChangeWeaponByNumberInputAction;

	//** Some input event functions */
	virtual void OnMoveInput(const FInputActionInstance& InputActionInstance);
	virtual void OnLookInput(const FInputActionInstance& InputActionInstance);
	virtual void OnChangeWeaponInput(const FInputActionInstance& InputActionInstance);
	virtual void OnChangeWeaponNumberInput(const FInputActionInstance& InputActionInstance);
	//** Life event functions */
	UFUNCTION()
	virtual void OnDamagedAny(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION()
	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;


	ABaseWeapon* CurrentWeapon;
	int32 CurrentWeaponId = 0;

	TArray<ABaseWeapon*> Weapons;
public:	

	//** Attributes */
	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
	ULivingAttributeComponent* LivingAttributeComponent;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	ULivingAttributeComponent* GetLivingAttributeComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ABaseWeapon* GetCurrentWeaponActor();
};
