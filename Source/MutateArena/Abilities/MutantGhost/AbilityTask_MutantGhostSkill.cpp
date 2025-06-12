#include "AbilityTask_MutantGhostSkill.h"

UAbilityTask_MutantGhostSkill* UAbilityTask_MutantGhostSkill::CreateTask(UGameplayAbility* OwningAbility, float SkillDuration)
{
	UAbilityTask_MutantGhostSkill* MyObj = NewAbilityTask<UAbilityTask_MutantGhostSkill>(OwningAbility);
	MyObj->SkillDuration = SkillDuration;

	return MyObj;
}

void UAbilityTask_MutantGhostSkill::Activate()
{
	Super::Activate();

	// 技能持续时间定时器
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::EndSkill, SkillDuration);
}

void UAbilityTask_MutantGhostSkill::EndSkill()
{
	OnSkillEnd.Broadcast();
}

void UAbilityTask_MutantGhostSkill::OnDestroy(bool bInOwnerFinished)
{
	// 清除定时器
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	Super::OnDestroy(bInOwnerFinished);
}
