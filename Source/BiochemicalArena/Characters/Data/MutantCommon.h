#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MutantCommon.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutantCommon : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayAbilityBase> ChangeMutantAbility;

};
