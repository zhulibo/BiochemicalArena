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
	class UNiagaraSystem* ExplodeEffect;
	UPROPERTY()
	class UNiagaraComponent* ExplodeEffectComponent;
	UPROPERTY(EditAnywhere)
	class UMetaSoundSource* ExplodeSound;

	UPROPERTY()
	float Damage = 500.f;
	UPROPERTY()
	float DamageInnerRadius = 1.f;
	UPROPERTY()
	float DamageOuterRadius = 5.f;
	void ExplodeDamage();

};
