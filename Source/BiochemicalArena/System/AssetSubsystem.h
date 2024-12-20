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
	class UCommonAsset* CommonAsset;
	UPROPERTY()
	class UInputBase* InputBase;
	UPROPERTY()
	class UInputHuman* InputHuman;
	UPROPERTY()
	class UInputMutant* InputMutant;
	UPROPERTY()
	class UInputSpectator* InputSpectator;

	UPROPERTY()
	class UCharacterSound* CharacterSound;
	UPROPERTY()
	class UMutantCommon* MutantCommon;

	void LoadCompleted();

	void Test();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 资源命名必须是 Prefix + 类名
	FPrimaryAssetId SystemSoundId;
	FPrimaryAssetId CommonAssetId;
	FPrimaryAssetId InputBaseId;
	FPrimaryAssetId InputHumanId;
	FPrimaryAssetId InputMutantId;
	FPrimaryAssetId InputSpectatorId;

	FPrimaryAssetId CharacterSoundId;
	FPrimaryAssetId MutantCommonId;

	UPROPERTY()
	FString Prefix = TEXT("DA_");
	FPrimaryAssetId GenerateId(UClass* Class);

};
