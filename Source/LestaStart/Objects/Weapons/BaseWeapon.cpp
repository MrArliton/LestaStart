
#include "BaseWeapon.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "LestaStart/Core/LestaCharacter.h"


ABaseWeapon::ABaseWeapon() 
{	
	// Replication 
	bReplicates = true;
	NetUpdateFrequency = 10;
	SetReplicateMovement(true);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	Mesh->SetIsReplicated(true);
	this->SetRootComponent(Mesh);
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, IsActive);
}

void ABaseWeapon::AttachWeapon(ALestaCharacter* Character, FName SocketName)
{	
	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot attach weapon to character, character is invalid, object: %s"), *this->GetName());
		return;
	}
	SetActorEnableCollision(false);	

	if (HasAuthority())
	{
		// Need for correct working rpc
		SetOwner(Character);
		/** Attach weapon to character */
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		this->AttachToComponent(Character->GetMesh(), AttachmentTransformRules, SocketName);
	}
}

void ABaseWeapon::PreDetachWeapon()
{
	SetActorEnableCollision(true);
}

void ABaseWeapon::DetachWeapon(bool AutoDestoy)
{
	// Call this function before detach logic
	PreDetachWeapon();

	SetOwner(nullptr);

	if (HasAuthority())
	{
		if (AutoDestoy)
		{
			Destroy();
		}
		else
		{
			FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
			DetachFromActor(DetachRules);
		}
	}
}

void ABaseWeapon::SetupInputComponent(UInputComponent* NewInputComponent)
{
	// Clear old input component and setup new 
	ClearInputComponent();
	InputComponent = NewInputComponent;
}

void ABaseWeapon::ClearInputComponent()
{
	if (IsValid(InputComponent))
	{
		/** Remove bindings in the input component */
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		if (EIC)
		{
			EIC->ClearBindingsForObject(this);
		}
	}
	InputComponent = nullptr;

}

/** Resumes response to input */
void ABaseWeapon::Activate()
{
	if (IsValid(Mesh))
	{
		Mesh->SetVisibility(true);
	}
	if (HasAuthority())
	{
		IsActive = true;
	}
}


/** Blocks response to input */
void ABaseWeapon::Deactivate(bool IsVisible)
{
	if (IsValid(Mesh))
	{
		GetMesh()->SetVisibility(IsVisible);
	}

	if (HasAuthority())
	{
		IsActive = false;
	}
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
