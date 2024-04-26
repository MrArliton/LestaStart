
#include "BaseWeapon.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LestaStart/Core/LestaCharacter.h"


ABaseWeapon::ABaseWeapon() 
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	this->SetRootComponent(Mesh);
}
/** When binding actions, be sure to add a handle using AddBindingHandle */
void ABaseWeapon::AttachWeapon(ALestaCharacter* Character, FName SocketName)
{
	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot attach weapon to character, character is invalid, object: %s"), *this->GetName());
		return;
	}
	OwnerCharacter = Character;
	
	/** Attach weapon to character */
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	this->AttachToComponent(Character->GetMesh(), AttachmentTransformRules, SocketName);

	/** For Special mapping context */
	if (InputMappingContext)
	{
		if (APlayerController* Controller = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
			{
				/** Set priority 1 due to possible click overlap */
				InputSubsystem->AddMappingContext(InputMappingContext, MappingPriority);
			}
		}
	}
}
void ABaseWeapon::Detach()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	this->DetachFromActor(DetachRules);
	if (IsValid(OwnerCharacter))
	{
		if (APlayerController* Controller = Cast<APlayerController>(OwnerCharacter->GetController()))
		{
			/** Remove bindings in the input component */
			UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(Controller->InputComponent);
			if (EIC)
			{
				EIC->ClearBindingsForObject(this);
			}
		}
	}
}

/** Resumes response to input */
void ABaseWeapon::Activate()
{
	IsActive = true;
	this->SetActorHiddenInGame(false);
}

/** Blocks response to input */
void ABaseWeapon::Deactivate(bool HideActor) 
{
	IsActive = false;
	this->SetActorHiddenInGame(HideActor);
}


bool ABaseWeapon::IsActivated()
{
	return IsActive;
}

USkeletalMeshComponent* ABaseWeapon::GetMesh()
{
	return Mesh;
}

/** Override this function in inherited classes */
bool ABaseWeapon::IsAttacking()
{
	return false; 
}
