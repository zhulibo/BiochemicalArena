#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	if (HasAuthority())
	{
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleSocket"));
		if (MuzzleSocket)
		{
			FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
			FVector ToTarget = HitTarget - SocketTransform.GetLocation();
			FRotator TargetRotation = ToTarget.Rotation();
			if (ProjectileClass && InstigatorPawn)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = InstigatorPawn;
				UWorld* World = GetWorld();
				if (World)
				{
					World->SpawnActor<AProjectile>(
						ProjectileClass,
						SocketTransform.GetLocation(),
						TargetRotation,
						SpawnParams
						);
				}
			}
		}
	}

	Super::Fire(HitTarget);
}
