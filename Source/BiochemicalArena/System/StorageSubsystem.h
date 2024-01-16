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
	void CreatePlayerStorage();
	UPROPERTY()
	class UPlayerStorage* PlayerStorageCache; // 存档缓存
	void Save();

	void InitDefaultSetting();

	void SetCharacterControlVariable();

	void SetAudio(float Value);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	FString SlotName = TEXT("Slot1");
	UPROPERTY()
	uint32 UserIndex = 0;

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	FTimerHandle WriteFileTimerHandle;
	void WriteFile();
	void OnWriteFileComplete(bool bWasSuccessful);

	float MapSensitivity(float Value);

	UPROPERTY()
	USoundMix* SoundMix;
	FAudioDeviceHandle AudioDevice;
	UPROPERTY()
	USoundClass* MasterClass;

public:
	FORCEINLINE FString GetSlotName() const { return SlotName; }
	FORCEINLINE uint32 GetUserIndex() const { return UserIndex; }

};
