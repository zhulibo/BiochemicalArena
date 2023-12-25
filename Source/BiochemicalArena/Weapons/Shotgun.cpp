#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/Projectile.h"

AShotgun::AShotgun()
{
	WeaponCate = EWeaponCate::Shotgun;
}

void AShotgun::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleSocket"));
	if (MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				for (uint32 i = 0; i < PelletNum; ++i)
				{
					FVector ToTargetRandom = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ToTarget, SpreadDegrees);
					AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(
						ProjectileClass,
						SocketTransform.GetLocation(),
						ToTargetRandom.Rotation(),
						Params
					);
					SpawnedProjectile->Damage = Damage;
				}
			}
		}
	}
}
