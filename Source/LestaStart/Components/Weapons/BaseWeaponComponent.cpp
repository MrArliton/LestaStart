
#include "BaseWeaponComponent.h"
#include "Net/UnrealNetwork.h"

UBaseWeaponComponent::UBaseWeaponComponent()
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bCanEverTick = true;
}



void UBaseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	Ammo = MaxAmmo;
}

void UBaseWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseWeaponComponent, Ammo);
	DOREPLIFETIME(UBaseWeaponComponent, IsAttacking);
}


void UBaseWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UBaseWeaponComponent::OnRep_Ammo()
{
	OnChangeAmmo.Broadcast(Ammo);
}

void UBaseWeaponComponent::ConsumeAmmo(float Amount)
{
	if (!bInfiniteAmmo)
	{
		Ammo = FMath::Clamp(Ammo - Amount, 0.0f, MaxAmmo);
		OnChangeAmmo.Broadcast(Ammo);
		if (Ammo == 0.0f)
		{
			Server_EndAttack();
		}
	}
}

void UBaseWeaponComponent::RestoreAmmo(float Amount)
{
	Ammo = FMath::Clamp(Ammo + Amount, 0.0f, MaxAmmo);
	OnChangeAmmo.Broadcast(Ammo);
}

void UBaseWeaponComponent::OnRep_IsAttacking()
{

}

void UBaseWeaponComponent::Server_StartAttack_Implementation() 
{
	if (Ammo > 0.0f || bInfiniteAmmo)
	{
		IsAttacking = true;
	}
}

void UBaseWeaponComponent::Server_EndAttack_Implementation()
{
	IsAttacking = false;
}

bool UBaseWeaponComponent::IsAttack()
{
	return IsAttacking;
}

void UBaseWeaponComponent::StartAttack(bool StartLocaly)
{
	/** On client start attack momentally */
	if (StartLocaly)
	{
		if (Ammo > 0.0f || bInfiniteAmmo)
		{
			IsAttacking = true;
		}
	}
	Server_StartAttack();
}

void UBaseWeaponComponent::EndAttack(bool EndLocaly)
{
	/** On client end attack */
	if (EndLocaly)
	{
		IsAttacking = false;
	}
	Server_EndAttack();
}

USceneComponent* UBaseWeaponComponent::GetDirectionComponent()
{
	return DirectionComponent;
}

void UBaseWeaponComponent::SetDirectionComponent(USceneComponent* Component)
{
	DirectionComponent = Component;
}

float UBaseWeaponComponent::GetCurrentAmmo()
{
	return Ammo;
}
float UBaseWeaponComponent::GetMaxAmmo()
{
	return MaxAmmo;
}