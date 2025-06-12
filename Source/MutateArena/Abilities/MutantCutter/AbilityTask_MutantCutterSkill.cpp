#include "AbilityTask_MutantCutterSkill.h"

UAbilityTask_MutantCutterSkill* UAbilityTask_MutantCutterSkill::CreateTask(UGameplayAbility* OwningAbility, float SkillDuration)
{
	UAbilityTask_MutantCutterSkill* MyObj = NewAbilityTask<UAbilityTask_MutantCutterSkill>(OwningAbility);
	MyObj->SkillDuration = SkillDuration;

	return MyObj;
}

void UAbilityTask_MutantCutterSkill::Activate()
{
	Super::Activate();

	// 技能持续时间定时器
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::EndSkill, SkillDuration);
}

void UAbilityTask_MutantCutterSkill::EndSkill()
{
	OnSkillEnd.Broadcast();
}

void UAbilityTask_MutantCutterSkill::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	Super::OnDestroy(bInOwnerFinished);
}
