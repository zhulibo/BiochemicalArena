﻿#pragma once

#include "StorageSaveGameType.generated.h"

USTRUCT(BlueprintType)
struct FBag
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString Primary;
	UPROPERTY()
	FString Secondary;
	UPROPERTY()
	FString Melee;
	UPROPERTY()
	FString Throwing;

};
