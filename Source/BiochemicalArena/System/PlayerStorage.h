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
	UPlayerStorage();

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
