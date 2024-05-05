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
class UTraceEstimatorComponent;

UCLASS()
class LESTASTART_API ALestaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALestaCharacter();
	/** Attach new weapon to character, only authority.
	* @param DefaultDeactivated - Deactivate and hide weapon after attaching */
	void AttachWeapon(ABaseWeapon* Weapon, bool DefaultDeactivated = true);

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FCharacterDelegateOne OnChangeWeapon;

protected:
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

	/** Default weapons that will spawn and attach when the game start. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponClasses;
	/** Socket of Character's SkeletalMesh where the weapon will be attached  */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;

public:	
	/** Represents the character's life - its health */
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Attribute")
	ULivingAttributeComponent* LivingAttributeComponent;

	/** Tracing to detect obstacles in front of the weapon  */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UTraceEstimatorComponent* AnimationTraceComponent;

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	ULivingAttributeComponent* GetLivingAttributeComponent();

	/** Current active weapon */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ABaseWeapon* GetCurrentWeaponActor();
	
	/** Special functon for using in animation blueprints 
	 * Return rotation for character animation */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	FRotator GetAnimationRotation();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/** Current weapon id (index in "Weapons" array) */
	UPROPERTY(Replicated)
	int32 CurrentWeaponId = 0;

	/** Current active weapon */
	UPROPERTY(Replicated)
	ABaseWeapon* CurrentWeapon;

	UFUNCTION()
	void OnRep_Weapons();
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Weapons)
	TArray<ABaseWeapon*> Weapons;

	/** Weapon change logic functions */
	UFUNCTION(Server, Unreliable)
	void Server_ChangeWeapon(int32 NewWeaponID);
	void ChangeWeapon(int32 NewWeaponID);

	/** Rotation for character animations
	 * "AnimationRotation" copied to server by 'server rpc', and server replicate it on other clients. */
	UPROPERTY(Replicated)
	FRotator AnimationRotation;
	/** Update AnimationRotation used by animation in the character */
	void UpdateAnimationRotation();

	/** Create all weapons from "WeaponClasses", attach them to WeaponSocketName and hide all except first one. */
	void SpawmAndAttachDefaultWeapons();
};
