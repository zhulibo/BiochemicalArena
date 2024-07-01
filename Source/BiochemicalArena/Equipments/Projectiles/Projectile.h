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

	UPROPERTY()
	float InitialSpeed = 60000.f;
	UPROPERTY()
	float Damage;

protected:
	virtual void PostActorCreated() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

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
	class USoundCue* ImpactSound;

	// 弹孔
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ImpactDecal;

public:
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

};
