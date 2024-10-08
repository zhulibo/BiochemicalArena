#include "ProjectileBullet.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/Equipments/Data/DamageTypeEquipment.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
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

	SetLifeSpan(1.f);

	CollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnHit);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// UE_LOG(LogTemp, Warning, TEXT("AProjectileBullet::OnHit Location: %s"), *Hit.ImpactPoint.ToString());
	// UE_LOG(LogTemp, Warning, TEXT("AProjectileBullet::OnHit Rotation: %s"), *Hit.ImpactNormal.Rotation().ToString());

	ABaseCharacter* DamagedActor = Cast<ABaseCharacter>(OtherActor);

	if (HasAuthority() && DamagedActor && GetInstigator())
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			GetDamage(Hit.Distance),
			GetInstigator()->Controller,
			this,
			UDamageTypeEquipment::StaticClass()
		);
	}

	SetLifeSpan(5.f);

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (TracerEffectComponent)
	{
		TracerEffectComponent->DestroyComponent();
	}

	if (DamagedActor == nullptr)
	{
		if (ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				ImpactEffect,
				GetActorLocation(),
				GetActorRotation()
			);
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
				5.f
			);

			DecalComponent->SetFadeScreenSize(0.004f);
		}
	}
}
