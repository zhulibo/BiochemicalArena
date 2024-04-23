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

	/*
	 * 本类所有属性虽然都会上传到了云端，但云端数据读取下来之后只有Bags和Character会应用到本地，
	 * 其他属性因玩家设备不同，只采用本地数据。
	 */

	UPROPERTY()
	TArray<FBag> Bags;
	UPROPERTY()
	FString Character = "SAS";

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
