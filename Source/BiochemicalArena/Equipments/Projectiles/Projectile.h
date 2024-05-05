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

	UPROPERTY(EditAnywhere, Category = "Equipment")
	float InitialSpeed = 60000.f;
	UPROPERTY()
	float Damage = 60.f;

protected:
	virtual void PostActorCreated() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UProjectileMovementComponent* ProjectileMovement;

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
	class USoundCue* ImpactSound;

public:
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

};
