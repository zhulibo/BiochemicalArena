#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCue_MutantGhostSkill.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AGameplayCue_MutantGhostSkill : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()


public:
	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

protected:
	UPROPERTY()
	class AMutantGhost* MutantGhost;

};
