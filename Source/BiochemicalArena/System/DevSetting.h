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
	
};
