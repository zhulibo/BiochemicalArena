#pragma once

#include "CoreMinimal.h"
#include "Throwing.h"
#include "Grenade.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AGrenade : public AThrowing
{
	GENERATED_BODY()

public:
	AGrenade();

	virtual void ThrowOut() override;

protected:

	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplodeParticle;
	UPROPERTY(EditAnywhere)
	class USoundCue* ExplodeSound;

	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius;
	UPROPERTY(EditAnywhere)
	float DamageOuterRadius;
	void ExplodeDamage();

};
