#include "AbilityTask_MutantSmokerSkill.h"

UAbilityTask_MutantSmokerSkill* UAbilityTask_MutantSmokerSkill::CreateTask(UGameplayAbility* OwningAbility, float SkillDuration)
{
	UAbilityTask_MutantSmokerSkill* MyObj = NewAbilityTask<UAbilityTask_MutantSmokerSkill>(OwningAbility);
	MyObj->SkillDuration = SkillDuration;

	return MyObj;
}

void UAbilityTask_MutantSmokerSkill::Activate()
{
	Super::Activate();

	// 技能持续时间定时器
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::EndSkill, SkillDuration);
}

void UAbilityTask_MutantSmokerSkill::EndSkill()
{
	OnSkillEnd.Broadcast();
}

void UAbilityTask_MutantSmokerSkill::OnDestroy(bool bInOwnerFinished)
{
	// 清除定时器
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	Super::OnDestroy(bInOwnerFinished);
}
