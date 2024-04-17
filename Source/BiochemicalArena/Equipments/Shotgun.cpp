#include "Shotgun.h"

#include "EquipmentType.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/ProjectileBullet.h"

AShotgun::AShotgun()
{
	EquipmentCate = EEquipmentCate::Shotgun;
}

void AShotgun::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (OwnerTeam == ETeam::NoTeam) SetOwnerTeam();
	if (MuzzleSocket == nullptr) MuzzleSocket = GetEquipmentMesh()->GetSocketByName(FName("Muzzle"));

	if (ProjectileClass && HumanCharacter && OwnerTeam != ETeam::NoTeam && MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetEquipmentMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		if (ProjectileClass)
		{
			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = HumanCharacter;

			for (int32 i = 0; i < PelletNum; ++i)
			{
				FVector ToTargetRandom = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ToTarget, SpreadDegrees);
				AProjectileBullet* Projectile = GetWorld()->SpawnActor<AProjectileBullet>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					ToTargetRandom.Rotation(),
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
	}
}
