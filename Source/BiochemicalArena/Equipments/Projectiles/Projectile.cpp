#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectile::PostActorCreated()
{
	Super::PostActorCreated();

	// 尽早生成，避免OnHit时Component还未生成
	SpawnTracerEffect();
	SpawnTrailEffect();
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::SpawnTracerEffect()
{
	if (TracerEffect)
	{
		TracerEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TracerEffect,
			GetRootComponent(),
			FName(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
}

void AProjectile::SpawnTrailEffect()
{
	if (TrailEffect)
	{
		TrailEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailEffect,
			GetRootComponent(),
			FName(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
}
