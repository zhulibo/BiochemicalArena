#pragma once

#include "CoreMinimal.h"
// #include "EOSSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StorageSubsystem.generated.h"

enum class ESoundClassType : uint8;

UCLASS()
class BIOCHEMICALARENA_API UStorageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void CreateSaveGameSetting();
	UPROPERTY()
	class USaveGameSetting* CacheSetting;
	UPROPERTY()
	class USaveGameLoadout* CacheLoadout;
	void Save();

	void ApplySetting();

	void SetAudio(float Value, ESoundClassType SoundClassType);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	FString SettingSlotName = TEXT("Slot1");
	UPROPERTY()
	FString LoadoutSlotName = TEXT("Slot2");

	UPROPERTY()
	uint32 UserIndex = 0;

	// UPROPERTY()
	// UEOSSubsystem* EOSSubsystem;
	FTimerHandle SaveToCloudTimerHandle;
	void SaveToCloud();
	void OnWriteFileComplete(bool bWasSuccessful);

	UPROPERTY()
	class UAssetSubsystem* AssetSubsystem;
	FAudioDeviceHandle AudioDevice;

};
