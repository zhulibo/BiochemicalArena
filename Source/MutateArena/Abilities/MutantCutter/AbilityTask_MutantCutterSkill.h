#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MutantCutterSkill.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutterSkillEnd);

UCLASS()
class MUTATEARENA_API UAbilityTask_MutantCutterSkill : public UAbilityTask
{
	GENERATED_BODY()

public:
	FOnCutterSkillEnd OnSkillEnd;

	UFUNCTION()
	static UAbilityTask_MutantCutterSkill* CreateTask(UGameplayAbility* OwningAbility, float SkillDuration);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	float SkillDuration;
	FTimerHandle TimerHandle;
	void EndSkill();

};
