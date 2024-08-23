#pragma once

#include "CoreMinimal.h"
#include "EOSSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StorageSubsystem.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UStorageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void CreateStorageSaveGame();
	UPROPERTY()
	class UStorageSaveGame* StorageCache; // 存档缓存
	void Save();

	void ApplySetting();

	void SetAudio(float Value);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	FString SlotName = TEXT("Slot1");
	UPROPERTY()
	uint32 UserIndex = 0;

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	FTimerHandle SaveToCloudTimerHandle;
	void SaveToCloud();
	void OnWriteFileComplete(bool bWasSuccessful);

	UPROPERTY()
	class UAssetSubsystem* AssetSubsystem;
	FAudioDeviceHandle AudioDevice;

public:
	FORCEINLINE FString GetSlotName() const { return SlotName; }
	FORCEINLINE uint32 GetUserIndex() const { return UserIndex; }

};
