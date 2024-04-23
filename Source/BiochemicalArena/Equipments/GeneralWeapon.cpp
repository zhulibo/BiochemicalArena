#include "GeneralWeapon.h"

#include "EquipmentType.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "..\PlayerStates\TeamType.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Projectiles/ProjectileBullet.h"

AGeneralWeapon::AGeneralWeapon()
{
	EquipmentCate = EEquipmentCate::General;
}

void AGeneralWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (OwnerTeam == ETeam::NoTeam) SetOwnerTeam();
	if (MuzzleSocket == nullptr) MuzzleSocket = GetEquipmentMesh()->GetSocketByName(FName("Muzzle"));

	if (ProjectileClass && HumanCharacter && OwnerTeam != ETeam::NoTeam && MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetEquipmentMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = HumanCharacter;

		AProjectileBullet* Projectile = GetWorld()->SpawnActor<AProjectileBullet>(
			ProjectileClass,
			SocketTransform.GetLocation(),
			TargetRotation,
			SpawnParams
		);

		Projectile->Damage = Damage;

		switch (OwnerTeam)
		{
		case ETeam::Team1:
			Projectile->GetCollisionBox()->SetCollisionResponseToChannel(ECC_Team2SkeletalMesh, ECollisionResponse::ECR_Block);
			break;
		case ETeam::Team2:
			Projectile->GetCollisionBox()->SetCollisionResponseToChannel(ECC_Team1SkeletalMesh, ECollisionResponse::ECR_Block);
			break;
		}
	}
}
