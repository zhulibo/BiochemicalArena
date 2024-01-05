#pragma once

#include "CoreMinimal.h"
#include "Throwing.h"
#include "Grenade.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AGrenade : public AThrowing
{
	GENERATED_BODY()

public:
	virtual void ThrowOut() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Equipment")
	UParticleSystem* ExplodeParticle;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	USoundCue* ExplodeSound;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	float Damage = 400.f;
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 100.f;
	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 400.f;
	void ExplodeDamage();

	bool bManualDestroyed = false;
	virtual void ManualDestroy() override;
	virtual void Destroyed() override;

};
