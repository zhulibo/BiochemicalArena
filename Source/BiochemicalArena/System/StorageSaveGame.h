#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY()
	FString Character = "SAS";

	UPROPERTY()
	float MouseSensitivity = 50.f;
	UPROPERTY()
	FString MouseAimAssistSteering = "off";
	UPROPERTY()
	FString MouseAimAssistSlowdown = "off";
	UPROPERTY()
	float ControllerSensitivity = 50.f;
	UPROPERTY()
	FString ControllerAimAssistSteering = "on";
	UPROPERTY()
	FString ControllerAimAssistSlowdown = "on";

	UPROPERTY()
	float Brightness = 2.2f;

	UPROPERTY()
	float Volume = 50.f;

};
