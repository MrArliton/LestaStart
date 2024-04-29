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
		UE_LOG(LogTemp, Error, TEXT("Cannot attach the blast weapon, character is undefined, object: %s"), *this->GetName());
		return;
	}

	if (IsValid(BlastWeaponComponent))
	{
		BlastWeaponComponent->AddIgnoreActor(Character);
	}	
} 

void ABlastWeapon::PreDetachWeapon()
{
	Super::PreDetachWeapon();

	if (IsValid(BlastWeaponComponent))
	{
		BlastWeaponComponent->EndAttack();
		BlastWeaponComponent->RemoveIgnoreActor(GetOwner());
	}
}

void ABlastWeapon::SetupInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupInputComponent(NewInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(NewInputComponent);
	if (EIC)
	{
		EIC->BindAction(AttackInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnStartAttack);
		EIC->BindAction(AttackInputAction, ETriggerEvent::Completed, this, &ThisClass::OnEndAttack);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot attach the laser weapon, error in getting UEnhancedInputComponent, object: %s"), *this->GetName());
	}
}

void ABlastWeapon::Deactivate(bool IsVisible)
{
	Super::Deactivate(IsVisible);
	if (IsValid(BlastWeaponComponent))
	{
		BlastWeaponComponent->EndAttack();
	}
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
	if (IsActive && IsValid(BlastWeaponComponent))
	{
		BlastWeaponComponent->StartAttack();
	}
}

void ABlastWeapon::OnEndAttack(const FInputActionInstance& InputActionInstance) 
{
	if (IsActive && IsValid(BlastWeaponComponent))
	{
		BlastWeaponComponent->EndAttack();
	}
}