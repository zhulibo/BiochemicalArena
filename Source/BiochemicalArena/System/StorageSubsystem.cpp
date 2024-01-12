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

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex)) // 如果本地存档不存在，则创建本地存档
	{
		CreatePlayerStorage();
	}
	else // 如果本地存档存在，则放到缓存里
	{
		PlayerStorageCache = Cast<UPlayerStorage>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	}
}

// 创建玩家存档
void UStorageSubsystem::CreatePlayerStorage()
{
	UPlayerStorage* PlayerStorage = Cast<UPlayerStorage>(UGameplayStatics::CreateSaveGameObject(UPlayerStorage::StaticClass()));
	if (PlayerStorage)
	{
		// 设置默认背包
		FBag Bag;
		Bag.Primary = "AK47";
		Bag.Secondary = "Glock17";
		Bag.Melee = "Kukri";
		Bag.Throwing = "Grenade";
		for (int i = 0; i < 4; ++i)
		{
			PlayerStorage->Bags.Add(Bag);
		}

		// 设置默认角色
		PlayerStorage->Character = "SAS";

		// 保存存档至缓存
		PlayerStorageCache = PlayerStorage;

		// 保存存档至本地
		UGameplayStatics::AsyncSaveGameToSlot(PlayerStorageCache, SlotName, UserIndex);
	}
}

// 读取玩家存档
UPlayerStorage* UStorageSubsystem::GetPlayerStorage()
{
	if (PlayerStorageCache == nullptr)
	{
		PlayerStorageCache = Cast<UPlayerStorage>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	}
	return PlayerStorageCache;
}

// 同步云存档中的装备和角色到本地
void UStorageSubsystem::SyncServerPlayerStorageToLocal(UPlayerStorage* ServerPlayerStorage)
{
	if (PlayerStorageCache && ServerPlayerStorage)
	{
		PlayerStorageCache->Bags = ServerPlayerStorage->Bags;
		PlayerStorageCache->Character = ServerPlayerStorage->Character;

		UGameplayStatics::AsyncSaveGameToSlot(PlayerStorageCache, SlotName, UserIndex);
	}
}

// 保存背包
void UStorageSubsystem::SaveBag(TArray<FBag> Bags)
{
	if (PlayerStorageCache)
	{
		PlayerStorageCache->Bags = Bags;

		// Save to local immediately
		UGameplayStatics::AsyncSaveGameToSlot(PlayerStorageCache, SlotName, UserIndex);

		// Throttle save to Cloud
		GetWorld()->GetTimerManager().SetTimer(WriteFileTimerHandle, this, &ThisClass::WriteFile, 5.f);
	}
}

// 保存角色
void UStorageSubsystem::SaveCharacter(FString Character)
{
	if (PlayerStorageCache)
	{
		PlayerStorageCache->Character = Character;

		// Save to local immediately
		UGameplayStatics::AsyncSaveGameToSlot(PlayerStorageCache, SlotName, UserIndex);

		// Throttle save to Cloud
		GetWorld()->GetTimerManager().SetTimer(WriteFileTimerHandle, this, &ThisClass::WriteFile, 5.f);
	}
}

// 保存到云端
void UStorageSubsystem::WriteFile()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		if (PlayerStorageCache)
		{
			TArray<uint8> FileContents;
			FMemoryWriter MemoryWriter(FileContents, true);
			FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
			Ar.ArIsSaveGame = false; // 无论属性设否设置了UPROPERTY(SaveGame)，都将进行序列化
			Ar.ArNoDelta = true;
			PlayerStorageCache->Serialize(Ar);

			EOSSubsystem->WriteFile(SlotName, FileContents); // 将本地存档保存到云端
		}
	}
}

// 保存到云端完成
void UStorageSubsystem::OnWriteFileComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnWriteFileComplete: %d"), bWasSuccessful);
}
