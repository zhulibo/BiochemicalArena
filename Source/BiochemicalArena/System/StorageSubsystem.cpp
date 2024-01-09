#include "StorageSubsystem.h"
#include "PlayerStorage.h"
#include "PlayerStorageType.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

UStorageSubsystem::UStorageSubsystem()
{
}

void UStorageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnWriteFileComplete.AddUObject(this, &ThisClass::OnWriteFileComplete);
	}

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex)) // 如果本地存档不存在，则创建存档本地
	{
		CreatePlayerStorage();
	}
}

// 创建玩家存档
void UStorageSubsystem::CreatePlayerStorage()
{
	UPlayerStorage* PlayerStorage = Cast<UPlayerStorage>(UGameplayStatics::CreateSaveGameObject(UPlayerStorage::StaticClass()));
	if (PlayerStorage)
	{
		// 设置默认值
		FBag Bag;
		Bag.Primary = "Ak47";
		Bag.Secondary = "Glock17";
		Bag.Melee = "Kukri";
		Bag.Throwing = "Grenade";
		for (int i = 0; i < 4; ++i)
		{
			PlayerStorage->Bags.Add(Bag);
		}

		PlayerStorage->Character = "SAS";

		SaveLocalPlayerStorage(PlayerStorage);
	}
}

void UStorageSubsystem::SaveLocalPlayerStorage(UPlayerStorage* PlayerStorage)
{
	PlayerStorageCache = PlayerStorage; // 保存存档至缓存

	UGameplayStatics::AsyncSaveGameToSlot(PlayerStorage, SlotName, UserIndex);
}

// 读取玩家存档
UPlayerStorage* UStorageSubsystem::GetPlayerStorage()
{
	UPlayerStorage* PlayerStorage = Cast<UPlayerStorage>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	return PlayerStorage;
}

// 读取缓存的玩家存档
UPlayerStorage* UStorageSubsystem::GetPlayerStorageCache()
{
	if (PlayerStorageCache)
	{
		return PlayerStorageCache;
	}
	return GetPlayerStorage();
}

// 同步云存档中的部分配置到本地
void UStorageSubsystem::SyncServerPlayerStorageToLocal(UPlayerStorage* ServerPlayerStorage)
{
	UPlayerStorage* PlayerStorage = GetPlayerStorage();
	if (PlayerStorage && ServerPlayerStorage)
	{
		PlayerStorage->Bags = ServerPlayerStorage->Bags;
		PlayerStorage->Character = ServerPlayerStorage->Character;

		SaveLocalPlayerStorage(PlayerStorage);
	}
}

// 保存背包
void UStorageSubsystem::SaveBag(TArray<FBag> Bags)
{
	UPlayerStorage* PlayerStorage = GetPlayerStorage();
	if (PlayerStorage)
	{
		PlayerStorage->Bags = Bags;

		// Save to local immediately
		SaveLocalPlayerStorage(PlayerStorage);

		// Throttle save to Cloud
		GetWorld()->GetTimerManager().SetTimer(WriteFileTimerHandle, this, &ThisClass::WriteFile, 5.f);
	}
}

void UStorageSubsystem::WriteFile()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		UPlayerStorage* PlayerStorage = GetPlayerStorage(); // 读取本地存档
		if (PlayerStorage)
		{
			TArray<uint8> FileContents;
			FMemoryWriter MemoryWriter(FileContents, true);
			FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
			Ar.ArIsSaveGame = false; // 无论属性设否设置了UPROPERTY(SaveGame)，都将进行序列化
			Ar.ArNoDelta = true;
			PlayerStorage->Serialize(Ar);

			EOSSubsystem->WriteFile(SlotName, FileContents); // 将本地存档保存到云端
		}
	}
}

void UStorageSubsystem::OnWriteFileComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnWriteFileComplete: %d"), bWasSuccessful);
}
