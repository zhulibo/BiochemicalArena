#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameSetting.generated.h"

enum class EMutantCharacterName : uint8;
enum class EHumanCharacterName : uint8;
struct FBag;

UCLASS()
class BIOCHEMICALARENA_API USaveGameSetting : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGameSetting();

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

	UPROPERTY()
	FString Language;
	
	UPROPERTY()
	float MouseSensitivity;
	UPROPERTY()
	bool MouseAimAssistSteering;
	UPROPERTY()
	bool MouseAimAssistSlowdown;
	UPROPERTY()
	float ControllerSensitivity;
	UPROPERTY()
	bool ControllerAimAssistSteering;
	UPROPERTY()
	bool ControllerAimAssistSlowdown;

	EWindowMode::Type WindowMode;
	UPROPERTY()
	FIntPoint ScreenResolution;
	UPROPERTY()
	float Brightness;

	UPROPERTY()
	float MasterVolume;
	UPROPERTY()
	float EffectsVolume;
	UPROPERTY()
	float MusicVolume;
	UPROPERTY()
	float DialogueVolume;

};
