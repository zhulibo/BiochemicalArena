#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCue_MutantRestore.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AGameplayCue_MutantRestore : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AGameplayCue_MutantRestore();

	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

protected:
	UPROPERTY(EditAnywhere)
	class UMetaSoundSource* RestoreSound;
	void PlaySound();

	UPROPERTY()
	FTimerHandle TimerHandle;
	
	UPROPERTY()
	class AMutantCharacter* MutantCharacter;
	
};
