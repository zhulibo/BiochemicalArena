#include "GeneralWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectiles/Projectile.h"

AGeneralWeapon::AGeneralWeapon()
{
	WeaponCate = EWeaponCate::General;
}

void AGeneralWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleSocket"));
	if (MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					Params
				);
				SpawnedProjectile->Damage = Damage;
			}
		}
	}
}
