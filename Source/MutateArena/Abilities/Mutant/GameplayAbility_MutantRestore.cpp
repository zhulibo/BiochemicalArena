#include "GameplayAbility_MutantRestore.h"

#include "AbilitySystemComponent.h"
#include "MutateArena/Characters/MutantCharacter.h"
#include "MutateArena/Characters/Data/CharacterAsset.h"
#include "MutateArena/System/AssetSubsystem.h"

UGameplayAbility_MutantRestore::UGameplayAbility_MutantRestore()
{
}

void UGameplayAbility_MutantRestore::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantRestore::ActivateAbility"));

	if (AssetSubsystem == nullptr) AssetSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(ActorInfo->AvatarActor.Get());
	if (AssetSubsystem && AssetSubsystem->CharacterAsset && ASC && MutantCharacter)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(AssetSubsystem->CharacterAsset->MutantRestoreEffect, MutantCharacter->GetCharacterLevel(), EffectContext);
		if (SpecHandle.IsValid())
		{
			// 应用技能效果
			EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			// 消耗CD
			CommitAbility(Handle, ActorInfo, ActivationInfo);
		}
	}
}

bool UGameplayAbility_MutantRestore::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UGameplayAbility_MutantRestore::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantRestore::CancelAbility"));

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_MutantRestore::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("UGameplayAbility_MutantRestore::EndAbility"));

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
