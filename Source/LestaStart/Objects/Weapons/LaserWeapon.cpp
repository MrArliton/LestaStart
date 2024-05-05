// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserWeapon.h"
#include "EnhancedInputComponent.h"
#include "LestaStart/Components/Weapons/LaserWeaponComponent.h"
#include "LestaStart/Core/LestaCharacter.h"
#include "Components/AudioComponent.h"

ALaserWeapon::ALaserWeapon()
{
	bNetUseOwnerRelevancy = true;

	LaserDirectionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Laser Direction"));
	LaserDirectionComponent->SetupAttachment(Mesh);

	LaserWeaponComponent = CreateDefaultSubobject<ULaserWeaponComponent>(TEXT("Laser Component"));
	LaserWeaponComponent->SetDirectionComponent(LaserDirectionComponent);
	LaserWeaponComponent->SetIsReplicated(true);         
}

void ALaserWeapon::SetupInputComponent(UInputComponent* NewInputComponent)
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

void ALaserWeapon::Deactivate(bool IsVisible)
{
	Super::Deactivate(IsVisible);

	if (IsValid(LaserWeaponComponent))
	{
		LaserWeaponComponent->EndAttack();
	}
}

bool ALaserWeapon::IsAttacking()
{
	if (IsValid(LaserWeaponComponent)) 
	{
		return LaserWeaponComponent->IsAttack();
	}

	return false;
}

void ALaserWeapon::OnStartAttack(const FInputActionInstance& InputActionInstance)
{
	if (IsActive)
	{
		if (IsValid(LaserWeaponComponent) && !LaserWeaponComponent->IsAttack())
		{
			LaserWeaponComponent->StartAttack();
		}
	}
}

void ALaserWeapon::OnEndAttack(const FInputActionInstance& InputActionInstance)
{
	if (IsValid(LaserWeaponComponent) && IsActive)
	{
		LaserWeaponComponent->EndAttack();
	}
}
