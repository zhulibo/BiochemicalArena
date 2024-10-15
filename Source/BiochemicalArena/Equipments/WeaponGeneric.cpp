#include "WeaponGeneric.h"

#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "..\PlayerStates\TeamType.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "Components/BoxComponent.h"
#include "Data/EquipmentType.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/ProjectileBullet.h"

AWeaponGeneric::AWeaponGeneric()
{
}

void AWeaponGeneric::Fire(const FVector& HitTarget, float RecoilVert, float RecoilHor)
{
	Super::Fire(HitTarget, RecoilVert, RecoilHor);

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (OwnerTeam == ETeam::NoTeam) SetOwnerTeam();
	if (MuzzleSocket == nullptr) MuzzleSocket = GetEquipmentMesh()->GetSocketByName(FName("Muzzle"));

	if (HumanCharacter == nullptr || ProjectileClass == nullptr|| OwnerTeam == ETeam::NoTeam || MuzzleSocket == nullptr) return;

	FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetEquipmentMesh());
	FRotator TargetRotation = (HitTarget - SocketTransform.GetLocation()).Rotation();

	// 应用后坐力（gun kick）
	TargetRotation.Pitch += RecoilVert;
	TargetRotation.Yaw += RecoilHor;

	// 添加散布
	float TempCenterSpread = CenterSpread;
	if (HumanCharacter->GetCombatComponent() && HumanCharacter->GetCombatComponent()->IsFirstShot())
	{
		// 第一发无散布
		TempCenterSpread = 0.f;
	}
	FVector ToTargetWithSpread = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(TargetRotation.Vector(), TempCenterSpread);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = HumanCharacter;

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
