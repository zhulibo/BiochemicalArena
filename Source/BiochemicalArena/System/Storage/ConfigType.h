#pragma once

#include "ConfigType.generated.h"

USTRUCT(BlueprintType)
struct FBag
{
	GENERATED_BODY()

	UPROPERTY()
	FString Primary;
	UPROPERTY()
	FString Secondary;
	UPROPERTY()
	FString Melee;
	UPROPERTY()
	FString Throwing;

};
