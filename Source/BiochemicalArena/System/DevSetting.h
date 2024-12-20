#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DevSetting.generated.h"

UCLASS(Config = EditorPerProjectUserSettings)
class BIOCHEMICALARENA_API UDevSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "EditorMaps", meta=(AllowedClasses="/Script/Engine.World"))
	TArray<FSoftObjectPath> EditorMaps;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	bool bUseDevSettings = true;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	int32 TotalRound = 3;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	float WarmupTime = 3.f;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	float RoundTime = 60.f;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	float MutateTime = 5.f;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	float PostRoundTime = 3.f;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	float CooldownTime = 3.f;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Mutation", meta=(AllowedClasses="/Script/Engine.World"))
	int32 MutateClientIndex = 2;

};
