#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AssetSubsystem.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UAssetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class USystemSound* SystemSound;
	UPROPERTY()
	class UCharacterSound* CharacterSound;
	UPROPERTY()
	class UMutantCommon* MutantCommon;

	void Test();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};
