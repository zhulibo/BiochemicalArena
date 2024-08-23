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
	UPROPERTY()
	float Impulse;

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
	class UNiagaraSystem* TracerEffect;
	UPROPERTY()
	class UNiagaraComponent* TracerEffectComponent;
	void SpawnTracerEffect();

	// 在空中留下的轨迹
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TrailEffect;
	UPROPERTY()
	UNiagaraComponent* TrailEffectComponent;
	void SpawnTrailEffect();

	// 击中效果
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ImpactEffect;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	// 弹孔
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> ImpactDecal;

public:
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

};
