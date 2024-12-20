#include "GameplayAbility_MutantChange.h"

UGameplayAbility_MutantChange::UGameplayAbility_MutantChange()
{
}

void UGameplayAbility_MutantChange::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantChange::ActivateAbility"));

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::TimerEnd, 10.0f);
}

bool UGameplayAbility_MutantChange::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UGameplayAbility_MutantChange::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantChange::CancelAbility"));

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_MutantChange::TimerEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGameplayAbility_MutantChange::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantChange::EndAbility"));

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
