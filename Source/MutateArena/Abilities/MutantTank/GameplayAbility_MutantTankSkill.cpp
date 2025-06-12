#include "GameplayAbility_MutantTankSkill.h"

#include "AbilitySystemComponent.h"
#include "AbilityTask_MutantTankSkill.h"
#include "MutateArena/Characters/Mutants/MutantTank.h"

UGameplayAbility_MutantTankSkill::UGameplayAbility_MutantTankSkill()
{
}

void UGameplayAbility_MutantTankSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantTankSkill::ActivateAbility"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AMutantTank* MutantTank = Cast<AMutantTank>(ActorInfo->AvatarActor.Get());
	if (ASC && MutantTank)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(MutantTank->SkillEffect, MutantTank->GetCharacterLevel(), EffectContext);
		if (SpecHandle.IsValid())
		{
			// 应用技能效果
			EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			// 消耗CD
			CommitAbility(Handle, ActorInfo, ActivationInfo);

			// 绑定技能结束回调
			if (HasAuthority(&ActivationInfo) && EffectHandle.IsValid())
			{
				float Duration = 0.f;
				if (const FActiveGameplayEffect* ActiveSkillEffect = ASC->GetActiveGameplayEffect(EffectHandle))
				{
					Duration = ActiveSkillEffect->Spec.Duration;
				}
				
				Task = UAbilityTask_MutantTankSkill::CreateTask(this, Duration);
				Task->OnSkillEnd.AddDynamic(this, &ThisClass::OnSkillEnd);
				Task->ReadyForActivation();
			}
		}
	}
}

bool UGameplayAbility_MutantTankSkill::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UGameplayAbility_MutantTankSkill::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantTankSkill::CancelAbility"));

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_MutantTankSkill::OnSkillEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantTankSkill::OnSkillEnd"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_MutantTankSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantTankSkill::EndAbility"));

	// 结束Task
	if (Task) Task->EndTask();

	// 移除技能效果
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (EffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(EffectHandle);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
