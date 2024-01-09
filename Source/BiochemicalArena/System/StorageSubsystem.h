#pragma once

#include "CoreMinimal.h"
#include "EOSSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StorageSubsystem.generated.h"

struct FBag;

UCLASS()
class BIOCHEMICALARENA_API UStorageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UStorageSubsystem();

	void CreatePlayerStorage();

	class UPlayerStorage* GetPlayerStorage();
	UPlayerStorage* GetPlayerStorageCache();

	void SyncServerPlayerStorageToLocal(UPlayerStorage* ServerPlayerStorage);

	void SaveBag(TArray<FBag> Bags);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	UPlayerStorage* PlayerStorageCache;
	void SaveLocalPlayerStorage(UPlayerStorage* PlayerStorage);

	UPROPERTY()
	FString SlotName = TEXT("Slot1");
	UPROPERTY()
	uint32 UserIndex = 0;

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	FTimerHandle WriteFileTimerHandle;
	void WriteFile();
	void OnWriteFileComplete(bool bWasSuccessful);

public:
	FORCEINLINE FString GetSlotName() const { return SlotName; }
	FORCEINLINE uint32 GetUserIndex() const { return UserIndex; }

};
