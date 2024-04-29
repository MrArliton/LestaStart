// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class ALestaCharacter;
class UInputMappingContext;

UCLASS()
class LESTASTART_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Attach weapon to Character mesh (to socket "SocketName") */
	virtual void AttachWeapon(ALestaCharacter* Character, FName SocketName);

	/** Deattach weapon from Character mesh 
	 * "AutoDestoy" - Set destroy weapon after detach */
	virtual void DetachWeapon(bool AutoDestoy = false);	

	/** Assign key bindings for weapon to new input component */
	virtual void SetupInputComponent(UInputComponent* NewInputComponent);
	/** Remove key bindings for weapon from input component */
	virtual void ClearInputComponent();

	/** Resumes response to input, make visible weapon (if it were hidden) */
	virtual void Activate();
	/** Blocks response to input
	* IsVisible - Make the weapon visible or invisible when disabled. */
	virtual void Deactivate(bool IsVisible = false);

	/** Override this function in inherited classes.
	 *  Default return false */
	virtual bool IsAttacking();

	virtual bool IsActivated();
	
	USkeletalMeshComponent* GetMesh();

protected:
	/** Automatically call by "DetachWeapon" method before "detach logic"  */
	virtual void PreDetachWeapon();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USkeletalMeshComponent* Mesh;

	/** InputComponent that controll weapon after attaching */
	UInputComponent* InputComponent;

	/** Use it in inherited classes to block player input. */
	UPROPERTY(Replicated)
	bool IsActive = false;
};
