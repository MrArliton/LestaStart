// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserWeapon.h"
#include "EnhancedInputComponent.h"
#include "LestaStart/Components/Weapons/LaserWeaponComponent.h"
#include "LestaStart/Core/LestaCharacter.h"
#include "Components/AudioComponent.h"

ALaserWeapon::ALaserWeapon()
{
	/** Indicate the start positions of the laser by using Scene Component */
	LaserDirectionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Laser Direction"));
	LaserDirectionComponent->SetupAttachment(Mesh);

	LaserWeaponComponent = CreateDefaultSubobject<ULaserWeaponComponent>(TEXT("Laser Component"));
	LaserWeaponComponent->SetDirectionComponent(LaserDirectionComponent);
                 
	/** Sound effects */
	OverheatSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Overheat Sound Effect"));
	OverheatSoundComponent->bAutoActivate = false;
	OverheatSoundComponent->SetActive(false);
	OverheatSoundComponent->SetupAttachment(Mesh);
	LaserSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Laser Sound Effect"));
	LaserSoundComponent->bAutoActivate = false;
	LaserSoundComponent->SetActive(false);
	LaserSoundComponent->SetupAttachment(Mesh);

	/** Don't attach to component, because we will set location to end of attack laser */
	SparkSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Spark Sound Effect"));  
}

void ALaserWeapon::AttachWeapon(ALestaCharacter* Character, FName SocketName)
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
		if (!LaserWeaponComponent->IsAttack())
		{
			LaserWeaponComponent->StartAttack();
			if (LaserWeaponComponent->IsAttack())
			{
				LaserSoundComponent->FadeIn(0.5f);
			}
		}
	}
}

void ALaserWeapon::OnEndAttack(const FInputActionInstance& InputActionInstance)
{
	if (IsActive)
	{
		LaserSoundComponent->FadeOut(0.5f, 0.0f);
		LaserWeaponComponent->EndAttack();
	}
}

ULaserWeaponComponent* ALaserWeapon::GetLaserWeaponComponent()
{
	return LaserWeaponComponent;
}