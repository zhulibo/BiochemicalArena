#include "Weapon.h"

#include "EquipmentAnimInstance.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "Components/SphereComponent.h"
#include "Shells/Shell.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	CollisionSphere->BodyInstance.bLockXRotation = true;
	CollisionSphere->BodyInstance.bLockYRotation = true;
	CollisionSphere->BodyInstance.bLockZRotation = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	MuzzleSocket = EquipmentMesh->GetSocketByName(FName("Muzzle"));

	CarriedAmmo = MaxCarriedAmmo;
	Ammo = MagCapacity;
}

void AWeapon::Fire(const FVector& HitTarget, float RecoilVert, float RecoilHor)
{
	if (GetEquipmentAnimInstance())
	{
		GetEquipmentAnimInstance()->Montage_Play(FireMontage_E);
	}

	if (ShellClass)
	{
		const USkeletalMeshSocket* ShellEjectSocket = EquipmentMesh->GetSocketByName(FName("ShellEject"));
		if (ShellEjectSocket)
		{
			FTransform SocketTransform = ShellEjectSocket->GetSocketTransform(EquipmentMesh);

			FActorSpawnParameters SpawnParams;
			SpawnParams.bDeferConstruction = true;

			AShell* Shell = GetWorld()->SpawnActor<AShell>(
				ShellClass,
				SocketTransform.GetLocation(),
				SocketTransform.GetRotation().Rotator(),
				SpawnParams
			);

			if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
			if (HumanCharacter)
			{
				Shell->InitVelocity = HumanCharacter->GetVelocity();
			}

			UGameplayStatics::FinishSpawningActor(Shell, SocketTransform);
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
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(HumanCharacter->Controller);
		if (BaseController)
		{
			BaseController->SetHUDAmmo(Ammo);
		}
	}
}

void AWeapon::SetHUDCarriedAmmo()
{
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(HumanCharacter->Controller);
		if (BaseController)
		{
			BaseController->SetHUDCarriedAmmo(CarriedAmmo);
		}
	}
}

void AWeapon::MulticastFullAmmo_Implementation()
{
	SetAmmo(MagCapacity);
	SetCarriedAmmo(MaxCarriedAmmo);
}
