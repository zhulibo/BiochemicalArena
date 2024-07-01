#include "Melee.h"
#include "EquipmentType.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "..\Characters\Components\CombatStateType.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Components/CapsuleComponent.h"
#include "DamageTypes/EquipmentDamageType.h"
#include "Kismet/GameplayStatics.h"

AMelee::AMelee()
{
	EquipmentCate = EEquipmentCate::Melee;

	AttackCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackCapsule"));
	AttackCapsule->SetupAttachment(RootComponent);
	AttackCapsule->SetGenerateOverlapEvents(true);
	AttackCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnAttackCapsuleOverlap);
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	SetAttackCapsuleCollision();
}

void AMelee::EquipEquipment()
{
	Super::EquipEquipment();

	SetAttackCapsuleCollision();
}

void AMelee::SetAttackCapsuleCollision()
{
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		switch (OwnerTeam)
		{
		case ETeam::Team1:
			AttackCapsule->SetCollisionResponseToChannel(ECC_Team2SkeletalMesh, ECollisionResponse::ECR_Overlap);
			break;
		case ETeam::Team2:
			AttackCapsule->SetCollisionResponseToChannel(ECC_Team1SkeletalMesh, ECollisionResponse::ECR_Overlap);
			break;
		}
	}
}

void AMelee::SetAttackCollisionEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		AttackCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		AttackCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitEnemies.Empty();
	}
}

void AMelee::OnAttackCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	AHumanCharacter* InstigatorCharacter = Cast<AHumanCharacter>(GetOwner());
	if (InstigatorCharacter && !HitEnemies.Contains(OtherActor))
	{
		HitEnemies.Add(OtherActor);

		float Damage;
		if (InstigatorCharacter->GetCombatComponent() && InstigatorCharacter->GetCombatComponent()->GetCombatState() == ECombatState::LightAttacking)
		{
			Damage = LightAttackDamage;
		}
		else
		{
			Damage = HeavyAttackDamage;
		}
		UGameplayStatics::ApplyDamage(OtherActor, Damage, InstigatorCharacter->Controller, this, UEquipmentDamageType::StaticClass());
	}
}
