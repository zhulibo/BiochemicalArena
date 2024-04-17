#include "Weapon.h"

#include "EquipmentAnimInstance.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Cassings/Casing.h"
#include "Engine/SkeletalMeshSocket.h"

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	MuzzleSocket = EquipmentMesh->GetSocketByName(FName("Muzzle"));

	Ammo = MagCapacity;
	CarriedAmmo = MaxCarriedAmmo;
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (GetEquipmentAnimInstance())
	{
		GetEquipmentAnimInstance()->Montage_Play(FireMontage_E);
	}

	if (CasingClass)
	{
		const USkeletalMeshSocket* ShellEjectSocket = EquipmentMesh->GetSocketByName(FName("ShellEject"));
		if (ShellEjectSocket)
		{
			FTransform SocketTransform = ShellEjectSocket->GetSocketTransform(EquipmentMesh);
			GetWorld()->SpawnActor<ACasing>(
				CasingClass,
				SocketTransform.GetLocation(),
				SocketTransform.GetRotation().Rotator()
			);
		}
	}

	SpendRound();
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter && HumanCharacter->IsLocallyControlled())
	{
		SetHUDAmmo();
	}
}

void AWeapon::SetAmmo(int32 AmmoNum)
{
	Ammo = AmmoNum;
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter && HumanCharacter->IsLocallyControlled())
	{
		SetHUDAmmo();
	}
}

void AWeapon::SetCarriedAmmo(int32 AmmoNum)
{
	CarriedAmmo = AmmoNum;
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter && HumanCharacter->IsLocallyControlled())
	{
		SetHUDCarriedAmmo();
	}
}

void AWeapon::SetHUDAmmo()
{
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		if (HumanController == nullptr) HumanController = Cast<AHumanController>(HumanCharacter->Controller);
		if (HumanController)
		{
			HumanController->SetHUDAmmo(Ammo);
		}
	}
}

void AWeapon::SetHUDCarriedAmmo()
{
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		if (HumanController == nullptr) HumanController = Cast<AHumanController>(HumanCharacter->Controller);
		if (HumanController)
		{
			HumanController->SetHUDCarriedAmmo(CarriedAmmo);
		}
	}
}
