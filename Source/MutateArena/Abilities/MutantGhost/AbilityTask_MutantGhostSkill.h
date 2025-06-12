#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MutantGhostSkill.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGhostSkillEnd);

UCLASS()
class MUTATEARENA_API UAbilityTask_MutantGhostSkill : public UAbilityTask
{
	GENERATED_BODY()

public:
	FOnGhostSkillEnd OnSkillEnd;

	UFUNCTION()
	static UAbilityTask_MutantGhostSkill* CreateTask(UGameplayAbility* OwningAbility, float SkillDuration);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	float SkillDuration;
	FTimerHandle TimerHandle;
	void EndSkill();

};
