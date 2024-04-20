#include "ProjectileBullet.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = 100000.f;
	ProjectileMovementComponent->MaxSpeed = 100000.f;
}

// Don't do this for now, Avoid cast
// void AProjectileBullet::PostActorCreated()
// {
// 	Super::PostActorCreated();
//
// 	AWeapon* Weapon = Cast<AWeapon>(GetOwner());
// 	if (Weapon)
// 	{
// 		ProjectileMovementComponent->InitialSpeed = Weapon->GetProjectileSpeed();
// 		ProjectileMovementComponent->MaxSpeed = Weapon->GetProjectileSpeed();
// 		Damage = Weapon->GetDamage();
// 	}
// }

void AProjectileBullet::SetDamage(float TempDamage)
{
	Damage = TempDamage;
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}

	SetLifeSpan(2.f);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetInstigator());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}

	MulticastOnHit();
}

void AProjectileBullet::MulticastOnHit_Implementation()
{
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
}
