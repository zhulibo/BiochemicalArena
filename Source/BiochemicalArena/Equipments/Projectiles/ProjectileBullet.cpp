#include "ProjectileBullet.h"

#include "BiochemicalArena/Equipments/DamageTypes/EquipmentDamageType.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AProjectileBullet::AProjectileBullet()
{
	CollisionBox->SetBoxExtent(FVector(4.f, 2.f, 2.f));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(CollisionBox);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = InitialSpeed;
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnHit);

	// 设置生命周期
	SetLifeSpan(1.f);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// FVector_NetQuantize ImpactPoint = Hit.ImpactPoint;
	// UE_LOG(LogTemp, Warning, TEXT("ImpactPoint: %s"), *ImpactPoint.ToString());

	if (HasAuthority())
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetInstigator());
		if (OwnerCharacter)
		{
			AController* OwnerController = OwnerCharacter->Controller;
			if (OwnerController)
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UEquipmentDamageType::StaticClass());
			}
		}
	}

	// 发生碰撞延长生命周期，以便特效有足够时间展示
	SetLifeSpan(11.f);

	if (TracerComponent)
	{
		TracerComponent->DestroyComponent();
	}

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ImpactDecal)
	{
		auto DecalComponent = UGameplayStatics::SpawnDecalAttached(
			ImpactDecal,
			FVector(10.f),
			HitComp,
			NAME_None,
			Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation(),
			EAttachLocation::KeepWorldPosition,
			10.f
		);

		DecalComponent->SetFadeScreenSize(0.004f);
	}
}
