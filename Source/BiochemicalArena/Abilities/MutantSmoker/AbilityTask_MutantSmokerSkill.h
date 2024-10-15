#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MutantSmokerSkill.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSmokerSkillEnd);

UCLASS()
class BIOCHEMICALARENA_API UAbilityTask_MutantSmokerSkill : public UAbilityTask
{
	GENERATED_BODY()

public:
	FOnSmokerSkillEnd OnSkillEnd;

	UFUNCTION()
	static UAbilityTask_MutantSmokerSkill* CreateTask(UGameplayAbility* OwningAbility, float SkillDuration);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	float SkillDuration;
	FTimerHandle TimerHandle;
	void EndSkill();

};
