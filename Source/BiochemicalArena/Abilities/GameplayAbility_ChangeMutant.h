#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/Abilities/GameplayAbilityBase.h"
#include "GameplayAbility_ChangeMutant.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UGameplayAbility_ChangeMutant : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGameplayAbility_ChangeMutant();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY()
	FTimerHandle TimerHandle;
	UFUNCTION()
	void TimerEnd();

};
