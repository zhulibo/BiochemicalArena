#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameLoadout.generated.h"

enum class EMutantCharacterName : uint8;
enum class EHumanCharacterName : uint8;

UCLASS()
class BIOCHEMICALARENA_API USaveGameLoadout : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGameLoadout();

	UPROPERTY()
	int32 Version = 1;

	UPROPERTY()
	TArray<struct FLoadout> Loadouts;
	UPROPERTY()
	int32 LoadoutIndex;

	UPROPERTY()
	EHumanCharacterName HumanCharacterName;
	UPROPERTY()
	EMutantCharacterName MutantCharacterName;

};
