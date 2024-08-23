#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MutantTankSkill.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTankSkillEnd);

UCLASS()
class BIOCHEMICALARENA_API UAbilityTask_MutantTankSkill : public UAbilityTask
{
	GENERATED_BODY()

public:
	FOnTankSkillEnd OnSkillEnd;

	UFUNCTION()
	static UAbilityTask_MutantTankSkill* CreateTask(UGameplayAbility* OwningAbility, float SkillDuration);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	float SkillDuration;
	FTimerHandle TimerHandle;
	void EndSkill();

};
