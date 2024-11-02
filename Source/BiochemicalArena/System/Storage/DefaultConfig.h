#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "ConfigType.h"
#include "DefaultConfig.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UDefaultConfig : public UObject
{
	GENERATED_BODY()

public:
	UDefaultConfig();

	UPROPERTY()
	FString DefaultPrimary = "AK47";
	UPROPERTY()
	FString DefaultSecondary = "Glock17";
	UPROPERTY()
	FString DefaultMelee = "Kukri";
	UPROPERTY()
	FString DefaultThrowing = "Grenade";
	UPROPERTY()
	TArray<FBag> Bags;
	int32 CurLoadoutIndex = 0;

	UPROPERTY()
	EHumanCharacterName HumanCharacterName = EHumanCharacterName::SAS;
	UPROPERTY()
	EMutantCharacterName MutantCharacterName = EMutantCharacterName::Tank;

	UPROPERTY()
	FString Language;
	UPROPERTY()
	bool ObfuscatePlayerName = false;
	UPROPERTY()
	bool ObfuscateTextChat = false;

	UPROPERTY()
	float MouseSensitivity = 0.8f;
	UPROPERTY()
	bool MouseAimAssistSteering = false;
	UPROPERTY()
	bool MouseAimAssistSlowdown = false;
	UPROPERTY()
	float ControllerSensitivity = 3.f;
	UPROPERTY()
	bool ControllerAimAssistSteering = true;
	UPROPERTY()
	bool ControllerAimAssistSlowdown = true;

	EWindowMode::Type WindowMode = EWindowMode::WindowedFullscreen;
	UPROPERTY()
	FIntPoint ScreenResolution = FIntPoint(1280, 720);
	UPROPERTY()
	float Brightness = 2.2f;

	UPROPERTY()
	float MasterVolume = 0.8f;
	UPROPERTY()
	float EffectsVolume = 1.f;
	UPROPERTY()
	float MusicVolume = 1.f;
	UPROPERTY()
	float DialogueVolume = 1.f;

};
