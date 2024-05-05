#include "Melee.h"
#include "EquipmentType.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "..\Characters\Components\CombatStateType.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Kismet/GameplayStatics.h"

AMelee::AMelee()
{
	EquipmentCate = EEquipmentCate::Melee;

	EquipmentMesh->SetGenerateOverlapEvents(true);
	EquipmentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	EquipmentMesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnMeshOverlap);
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	SetMeshCollision();
}

void AMelee::EquipEquipment()
{
	Super::EquipEquipment();

	SetMeshCollision();
}

void AMelee::SetMeshCollision()
{
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter && HumanCharacter->IsLocallyControlled())
	{
		switch (OwnerTeam)
		{
		case ETeam::Team1:
			EquipmentMesh->SetCollisionResponseToChannel(ECC_Team2SkeletalMesh, ECollisionResponse::ECR_Overlap);
			break;
		case ETeam::Team2:
			EquipmentMesh->SetCollisionResponseToChannel(ECC_Team1SkeletalMesh, ECollisionResponse::ECR_Overlap);
			break;
		}
	}
}

void AMelee::EnableMeshCollision(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitEnemies.Empty();
	}
}

void AMelee::OnMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHumanCharacter* InstigatorHumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (InstigatorHumanCharacter && InstigatorHumanCharacter->IsLocallyControlled() && !HitEnemies.Contains(OtherActor))
	{
		HitEnemies.Add(OtherActor);

		// 近战武器对Overlap的精度要求较高，所以在本地做伤害判定，再把结果传给服务器
		float DamageToApply;
		if (InstigatorHumanCharacter->GetCombat() && InstigatorHumanCharacter->GetCombat()->GetCombatState() == ECombatState::LightAttacking)
		{
			DamageToApply = LightAttackDamage;
		}
		else
		{
			DamageToApply = HeavyAttackDamage;
		}
		ServerApplyDamage(OtherActor, InstigatorHumanCharacter, DamageToApply);
	}
}


void AMelee::ServerApplyDamage_Implementation(AActor* OtherActor, AHumanCharacter* InstigatorHumanCharacter, float DamageToApply)
{
	UGameplayStatics::ApplyDamage(OtherActor, DamageToApply, InstigatorHumanCharacter->Controller, this, UDamageType::StaticClass());
}
