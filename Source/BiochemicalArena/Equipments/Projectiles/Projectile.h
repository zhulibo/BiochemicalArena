#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

protected:
	virtual void PostActorCreated() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
	float Damage;

	// 尾部效果
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;
	UPROPERTY()
	UParticleSystemComponent* TracerComponent;
	void SpawnTracer();

	// 在空中留下的轨迹
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* Trail; // TODO 实现效果
	UPROPERTY()
	class UNiagaraComponent* TrailComponent;
	void SpawnTrail();

	// 击中效果
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;

public:
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE float GetDamage() const { return Damage; }

};
