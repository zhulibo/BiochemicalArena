#include "GeneralWeapon.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerStates/Team.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectiles/Projectile.h"

AGeneralWeapon::AGeneralWeapon()
{
	EquipmentCate = EEquipmentCate::General;
}

void AGeneralWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (OwnerTeam == ETeam::NoTeam) SetOwnerTeam();
	if (MuzzleSocket == nullptr) MuzzleSocket = GetEquipmentMesh()->GetSocketByName(FName("MuzzleSocket"));

	if (ProjectileClass && HumanCharacter && OwnerTeam != ETeam::NoTeam && MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetEquipmentMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = HumanCharacter;

		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			SocketTransform.GetLocation(),
			TargetRotation,
			Params
		);

		Projectile->SetDamage(Damage);

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
