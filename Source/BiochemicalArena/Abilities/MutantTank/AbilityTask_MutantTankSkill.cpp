#include "AbilityTask_MutantTankSkill.h"

UAbilityTask_MutantTankSkill* UAbilityTask_MutantTankSkill::CreateTask(UGameplayAbility* OwningAbility, float SkillDuration)
{
	UAbilityTask_MutantTankSkill* MyObj = NewAbilityTask<UAbilityTask_MutantTankSkill>(OwningAbility);
	MyObj->SkillDuration = SkillDuration;

	return MyObj;
}

void UAbilityTask_MutantTankSkill::Activate()
{
	Super::Activate();

	// 技能持续时间定时器
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::EndSkill, SkillDuration);
}

void UAbilityTask_MutantTankSkill::EndSkill()
{
	OnSkillEnd.Broadcast();
}

void UAbilityTask_MutantTankSkill::OnDestroy(bool bInOwnerFinished)
{
	// 清除定时器
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	Super::OnDestroy(bInOwnerFinished);
}
