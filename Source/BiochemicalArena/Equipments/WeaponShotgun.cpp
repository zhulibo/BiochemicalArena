#include "WeaponShotgun.h"

#include "BiochemicalArena/Equipments/Data/EquipmentType.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/ProjectileBullet.h"

AWeaponShotgun::AWeaponShotgun()
{
}

void AWeaponShotgun::Fire(const FVector& HitTarget, float RecoilVert, float RecoilHor)
{
	Super::Fire(HitTarget, RecoilVert, RecoilHor);

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (OwnerTeam == ETeam::NoTeam) SetOwnerTeam();
	if (MuzzleSocket == nullptr) MuzzleSocket = GetEquipmentMesh()->GetSocketByName(FName("Muzzle"));

	if (ProjectileClass && HumanCharacter && OwnerTeam != ETeam::NoTeam && MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetEquipmentMesh());
		FRotator TargetRotation = (HitTarget - SocketTransform.GetLocation()).Rotation();

		// 应用后坐力（gun kick）
		TargetRotation.Pitch += RecoilVert;
		TargetRotation.Yaw += RecoilHor;

		if (ProjectileClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = HumanCharacter;

			for (int32 i = 0; i < PelletNum; ++i)
			{
				// 添加散布
				FVector ToTargetWithSpread = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(TargetRotation.Vector(), CenterSpread);

				AProjectileBullet* Projectile = GetWorld()->SpawnActor<AProjectileBullet>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					ToTargetWithSpread.Rotation(),
					SpawnParams
				);

				EEquipmentName TempEquipmentName = EquipmentParentName == EEquipmentName::NONE ? EquipmentName : EquipmentParentName;
				FString EnumValue = UEnum::GetValueAsString(TempEquipmentName);
				EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
				Projectile->OwnerName = EnumValue;

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
