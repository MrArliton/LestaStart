// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastWeapon.h"
#include "EnhancedInputComponent.h"
#include "LestaStart/Core/LestaCharacter.h"
#include "LestaStart/Components/Weapons/BlastWeaponComponent.h"

ABlastWeapon::ABlastWeapon() 
{
	BlastWeaponComponent = CreateDefaultSubobject<UBlastWeaponComponent>(TEXT("Blast Weapon"));
	/** Set center of attack blast in the center of mesh */
	BlastWeaponComponent->SetDirectionComponent(GetMesh());
}

void ABlastWeapon::AttachWeapon(ALestaCharacter* Character, FName SocketName)
{
	Super::AttachWeapon(Character, SocketName);

	if (!IsValid(Character))
	{
		return;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot attach the laser weapon, character is undefined, object: %s"), *this->GetName());
	}

	if (IsValid(BlastWeaponComponent))
	{
		BlastWeaponComponent->AddIgnoreActor(OwnerCharacter);
	}
	
	if (APlayerController* Controller = Cast<APlayerController>(Character->GetController()))
	{
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(Controller->InputComponent);
		if (EIC)
		{
			//** AddBindingHandle - for unbinding action when detach from character *
			EIC->BindAction(AttackInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnStartAttack).GetHandle();
			EIC->BindAction(AttackInputAction, ETriggerEvent::Completed, this, &ThisClass::OnEndAttack).GetHandle();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot attach the laser weapon, error in getting UEnhancedInputComponent, object: %s"), *this->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot attach the laser weapon, error in getting APlayerController, object: %s"), *this->GetName());
	}
} 

void ABlastWeapon::Detach()
{
	Super::Detach();

	if (IsValid(BlastWeaponComponent))
	{
		BlastWeaponComponent->RemoveIgnoreActor(OwnerCharacter);
	}
}

void ABlastWeapon::Deactivate(bool HideActor)
{
	Super::Deactivate(HideActor);

	BlastWeaponComponent->EndAttack();
}


bool ABlastWeapon::IsAttacking()
{
	if (IsValid(BlastWeaponComponent))
	{
		return BlastWeaponComponent->IsAttack();
	}
	return false;
}

void ABlastWeapon::OnStartAttack(const FInputActionInstance& InputActionInstance) 
{
	if (IsActive)
	{
		BlastWeaponComponent->StartAttack();
	}
}

void ABlastWeapon::OnEndAttack(const FInputActionInstance& InputActionInstance) 
{
	if (IsActive)
	{
		BlastWeaponComponent->EndAttack();
	}
}