#include "GameplayAbility_MutantSmokerSkill.h"

#include "AbilitySystemComponent.h"
#include "AbilityTask_MutantSmokerSkill.h"
#include "BiochemicalArena/Characters/MutantCharacters/MutantSmoker.h"

UGameplayAbility_MutantSmokerSkill::UGameplayAbility_MutantSmokerSkill()
{
}

void UGameplayAbility_MutantSmokerSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantSmokerSkill::ActivateAbility"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AMutantSmoker* MutantSmoker = Cast<AMutantSmoker>(ActorInfo->AvatarActor.Get());
	if (ASC && MutantSmoker)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(MutantSmoker->SkillEffect, MutantSmoker->GetCharacterLevel(), EffectContext);
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

				Task = UAbilityTask_MutantSmokerSkill::CreateTask(this, Duration);
				Task->OnSkillEnd.AddDynamic(this, &ThisClass::OnSkillEnd);
				Task->ReadyForActivation();
			}
		}
	}
}

bool UGameplayAbility_MutantSmokerSkill::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UGameplayAbility_MutantSmokerSkill::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	// UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantSmokerSkill::CancelAbility"));

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_MutantSmokerSkill::OnSkillEnd()
{
	// UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantSmokerSkill::OnSkillEnd"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_MutantSmokerSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantSmokerSkill::EndAbility"));

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
