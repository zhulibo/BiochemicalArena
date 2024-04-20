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

	UPROPERTY()
	class UBoxComponent* CollisionBox;
	UPROPERTY()
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY()
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
	float Damage;

	// 尾部效果
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UParticleSystem* Tracer;
	UPROPERTY()
	UParticleSystemComponent* TracerComponent;
	void SpawnTracer();

	// 在空中留下的轨迹
	UPROPERTY(EditAnywhere, Category = "Equipment")
	class UNiagaraSystem* Trail; // TODO 实现效果
	UPROPERTY()
	class UNiagaraComponent* TrailComponent;
	void SpawnTrail();

	// 击中效果
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	USoundCue* ImpactSound;

public:
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE float GetDamage() const { return Damage; }

};
