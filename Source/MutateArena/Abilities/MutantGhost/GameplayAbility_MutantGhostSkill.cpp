#include "GameplayAbility_MutantGhostSkill.h"

#include "AbilitySystemComponent.h"
#include "AbilityTask_MutantGhostSkill.h"
#include "MutateArena/Characters/Mutants/MutantGhost.h"

UGameplayAbility_MutantGhostSkill::UGameplayAbility_MutantGhostSkill()
{
}

void UGameplayAbility_MutantGhostSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantGhostSkill::ActivateAbility"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AMutantGhost* MutantGhost = Cast<AMutantGhost>(ActorInfo->AvatarActor.Get());

	if (ASC && MutantGhost)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(MutantGhost->SkillEffect, MutantGhost->GetCharacterLevel(), EffectContext);
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

				Task = UAbilityTask_MutantGhostSkill::CreateTask(this, Duration);
				Task->OnSkillEnd.AddDynamic(this, &ThisClass::OnSkillEnd);
				Task->ReadyForActivation();
			}
		}
	}
}

bool UGameplayAbility_MutantGhostSkill::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UGameplayAbility_MutantGhostSkill::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantGhostSkill::CancelAbility"));

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_MutantGhostSkill::OnSkillEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantGhostSkill::OnSkillEnd"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_MutantGhostSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantGhostSkill::EndAbility"));

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
