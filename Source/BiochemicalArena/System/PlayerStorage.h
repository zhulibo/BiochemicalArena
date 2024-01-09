#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerStorage.generated.h"

struct FBag;

UCLASS()
class BIOCHEMICALARENA_API UPlayerStorage : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FBag> Bags;
	UPROPERTY()
	FString Character;

	UPROPERTY()
	float MouseSensitivityValue = 50.f;

};
