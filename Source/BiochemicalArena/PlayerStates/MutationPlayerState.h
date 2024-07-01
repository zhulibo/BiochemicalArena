#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "MutationPlayerState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMutationPlayerState : public ABasePlayerState
{
	GENERATED_BODY()

public:
	virtual void SetTeam(ETeam TempTeam) override;

	virtual void AddDamage(float TempDamage) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	class AMutationController* MutationController;

	virtual void OnRep_Team()override;

	virtual void OnRep_Damage() override;
	float BaseDamage = 0;
	void Show1000DamageIcon();

};
