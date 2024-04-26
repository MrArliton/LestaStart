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

	//** Attach weapon to Character mesh (to socket "SocketName") and give input controll to weapon */
	virtual void AttachWeapon(ALestaCharacter* Character, FName SocketName);
	//** Deattach weapon from Character mesh and take away input controll from weapon */
	virtual void Detach();

	/** Override this function in inherited classes.
	 *  Default return false */
	virtual bool IsAttacking();

	/** Resumes response to input and unhide actor (if he was hidden) */
	virtual void Activate();
	/** Blocks response to input */
	virtual void Deactivate(bool HideActor = false);

	virtual bool IsActivated();

	USkeletalMeshComponent* GetMesh();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USkeletalMeshComponent* Mesh;

	/** Actor who owns weapon after attaching */
	ALestaCharacter* OwnerCharacter;

	/** Input mapping context */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Input")
	UInputMappingContext* InputMappingContext;

	/** Input mapping priority */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Input", meta = (ClampMin = "0"))
	int32 MappingPriority = 1;

	/** Use it in inherited classes to block player input. */
	bool IsActive = false;};
