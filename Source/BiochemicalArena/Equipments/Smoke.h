#pragma once

#include "CoreMinimal.h"
#include "Throwing.h"
#include "Smoke.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ASmoke : public AThrowing
{
	GENERATED_BODY()

public:
	ASmoke();

	virtual void ThrowOut() override;

protected:
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ExplodeEffect;
	UPROPERTY()
	class UNiagaraComponent* ExplodeEffectComponent;
	UPROPERTY(EditAnywhere)
	class UMetaSoundSource* ExplodeSound;

	void Explode();

};
