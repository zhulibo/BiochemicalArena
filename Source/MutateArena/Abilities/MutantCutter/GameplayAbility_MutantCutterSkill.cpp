#include "GameplayAbility_MutantCutterSkill.h"

#include "AbilitySystemComponent.h"
#include "AbilityTask_MutantCutterSkill.h"
#include "MutateArena/Characters/Mutants/MutantCutter.h"

UGameplayAbility_MutantCutterSkill::UGameplayAbility_MutantCutterSkill()
{
}

void UGameplayAbility_MutantCutterSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantCutterSkill::ActivateAbility"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AMutantCutter* MutantCutter = Cast<AMutantCutter>(ActorInfo->AvatarActor.Get());
	if (ASC && MutantCutter)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(MutantCutter->SkillEffect, MutantCutter->GetCharacterLevel(), EffectContext);
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
				
				Task = UAbilityTask_MutantCutterSkill::CreateTask(this, Duration);
				Task->OnSkillEnd.AddDynamic(this, &ThisClass::OnSkillEnd);
				Task->ReadyForActivation();
			}
		}
	}
}

bool UGameplayAbility_MutantCutterSkill::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UGameplayAbility_MutantCutterSkill::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantCutterSkill::CancelAbility"));

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_MutantCutterSkill::OnSkillEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantCutterSkill::OnSkillEnd"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_MutantCutterSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantCutterSkill::EndAbility"));

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
