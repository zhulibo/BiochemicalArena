#include "Melee.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/Characters/Components/CombatState.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "Kismet/GameplayStatics.h"

AMelee::AMelee()
{
	EquipmentCate = EEquipmentCate::Melee;

	EquipmentMesh->SetGenerateOverlapEvents(true);
	EquipmentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	EquipmentMesh->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnMeshOverlap);
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	SetMeshCollision();
}

void AMelee::OnEquipped()
{
	Super::OnEquipped();

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
