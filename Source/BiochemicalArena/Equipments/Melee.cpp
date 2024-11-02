#include "Melee.h"

#include "DataRegistrySubsystem.h"
#include "BiochemicalArena/Equipments/Data/EquipmentType.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "..\Characters\Components\CombatStateType.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "Components/CapsuleComponent.h"
#include "Data/DamageTypeMelee.h"
#include "Kismet/GameplayStatics.h"

AMelee::AMelee()
{
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

	if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
	{
		FString EnumValue = UEnum::GetValueAsString(EquipmentName);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);

		FDataRegistryId DataRegistryId(DR_MELEE_DATA, FName(EnumValue));
		if (const FMeleeData* MeleeData = DRSubsystem->GetCachedItem<FMeleeData>(DataRegistryId))
		{
			LightAttackDamage = MeleeData->LightAttackDamage;
			HeavyAttackDamage = MeleeData->HeavyAttackDamage;
			MoveSpeedMul = MeleeData->MoveSpeedMul;
		}
	}

	SetAttackCapsuleCollision();
}

void AMelee::OnEquip()
{
	Super::OnEquip();

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
			AttackCapsule->SetCollisionResponseToChannel(ECC_TEAM2_MESH, ECollisionResponse::ECR_Overlap);
			break;
		case ETeam::Team2:
			AttackCapsule->SetCollisionResponseToChannel(ECC_TEAM1_MESH, ECollisionResponse::ECR_Overlap);
			break;
		}
	}
}

void AMelee::OnStartSwapOut()
{
	Super::OnStartSwapOut();

	SetAttackCollisionEnabled(false);
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

// TODO 不在服务端视线内有时没有碰撞
void AMelee::OnAttackCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UE_LOG(LogTemp, Warning, TEXT("OnAttackCapsuleOverlap %d"), GetLocalRole());

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

		DropBlood(OverlappedComponent, OtherActor, OtherComp, Damage);

		if (HasAuthority())
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, InstigatorCharacter->Controller, this, UDamageTypeMelee::StaticClass());
		}
	}
}

void AMelee::DropBlood(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, float Damage)
{
	ABaseCharacter* OverlappedCharacter = Cast<ABaseCharacter>(OtherActor);

	if (OverlappedCharacter == nullptr || OverlappedCharacter->BloodEffect_Melee == nullptr) return;

	TArray<FHitResult> TraceResults;

	auto Start = OverlappedComponent->GetComponentLocation();
	auto End = OverlappedCharacter->GetActorLocation();

	GetWorld()->SweepMultiByObjectType(
		TraceResults,
		Start,
		End,
		FQuat::Identity,
		FCollisionObjectQueryParams(OverlappedCharacter->GetMesh()->GetCollisionObjectType()),
		FCollisionShape::MakeSphere(10.f)
	);

	// DrawDebugLine(GetWorld(), Start, End, C_YELLOW, true);

	for (auto TraceResult : TraceResults)
	{
		if (TraceResult.GetComponent()->GetUniqueID() == OtherComp->GetUniqueID())
		{
			auto BloodEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				OverlappedCharacter->BloodEffect_Melee,
				TraceResult.ImpactPoint,
				TraceResult.ImpactNormal.Rotation()
			);

			BloodEffectComponent->SetVariableInt("ParticleCount", ULibraryCommon::GetBloodParticleCount(Damage));
			BloodEffectComponent->SetVariableLinearColor("Color", OverlappedCharacter->BloodColor);

			break;
		}
	}
}
