#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "GameFramework/SaveGame.h"
#include "StorageSaveGame.generated.h"

struct FBag;

UCLASS()
class BIOCHEMICALARENA_API UStorageSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UStorageSaveGame();

	UPROPERTY()
	TArray<FBag> Bags;
	int32 CurBagIndex = 0;

	UPROPERTY()
	FString HumanCharacter = "SAS";
	UPROPERTY()
	EMutantCharacterName MutantCharacterName = EMutantCharacterName::Tank;

	UPROPERTY()
	float MouseSensitivity = 1.f;
	UPROPERTY()
	bool MouseAimAssistSteering = false;
	UPROPERTY()
	bool MouseAimAssistSlowdown = false;
	UPROPERTY()
	float ControllerSensitivity = 1.f;
	UPROPERTY()
	bool ControllerAimAssistSteering = true;
	UPROPERTY()
	bool ControllerAimAssistSlowdown = true;

	UPROPERTY()
	float Brightness = 2.2f;

	UPROPERTY()
	float Volume = 0.8f;

};
