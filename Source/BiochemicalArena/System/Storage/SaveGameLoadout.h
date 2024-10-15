#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameLoadout.generated.h"

enum class EMutantCharacterName : uint8;
enum class EHumanCharacterName : uint8;
struct FBag;

UCLASS()
class BIOCHEMICALARENA_API USaveGameLoadout : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGameLoadout();

	UPROPERTY()
	int32 Version = 1;
	
	UPROPERTY()
	TArray<FBag> Bags;
	UPROPERTY()
	int32 CurLoadoutIndex;

	UPROPERTY()
	EHumanCharacterName HumanCharacterName;
	UPROPERTY()
	EMutantCharacterName MutantCharacterName;

};
