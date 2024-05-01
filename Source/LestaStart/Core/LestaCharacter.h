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

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	/** Attach new weapon to character, only authority.
	* @param DefaultDeactivated - Deactivate and hide weapon after attaching */
	void AttachWeapon(ABaseWeapon* Weapon, bool DefaultDeactivated = true);

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FCharacterDelegateOne OnChangeWeapon;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

private:
	UPROPERTY(Replicated)
	int32 CurrentWeaponId = 0;
	UPROPERTY(Replicated)
	ABaseWeapon* CurrentWeapon;
	
	UFUNCTION()
	void OnRep_Weapons();
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Weapons)
	TArray<ABaseWeapon*> Weapons;

	UFUNCTION(Server, Unreliable)
	void Server_ChangeWeapon(int32 NewWeaponID);

	void ChangeWeapon(int32 NewWeaponID);
	
	/** Rotation for character animations */
	UPROPERTY(Replicated)
	FRotator AnimationRotation;
public:	
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Attribute")
	ULivingAttributeComponent* LivingAttributeComponent;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	ULivingAttributeComponent* GetLivingAttributeComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ABaseWeapon* GetCurrentWeaponActor();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	FRotator GetAnimationRotation();
};
