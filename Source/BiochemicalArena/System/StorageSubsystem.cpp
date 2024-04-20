#include "StorageSubsystem.h"

#include "AssetSubsystem.h"
#include "AudioDevice.h"
#include "StorageSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

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
		CreateStorageSaveGame();
	}
	else // 如果本地存档存在，则放到缓存里
	{
		if (UStorageSaveGame* StorageSaveGame = Cast<UStorageSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)))
		{
			StorageCache = StorageSaveGame;

			ApplySetting();
		}
		else
		{
			CreateStorageSaveGame();
		}
	}
}

// 创建玩家存档
void UStorageSubsystem::CreateStorageSaveGame()
{
	UStorageSaveGame* StorageSaveGame = Cast<UStorageSaveGame>(UGameplayStatics::CreateSaveGameObject(UStorageSaveGame::StaticClass()));
	if (StorageSaveGame)
	{
		// 保存存档至缓存
		StorageCache = StorageSaveGame;

		ApplySetting();

		// 保存存档至本地
		UGameplayStatics::SaveGameToSlot(StorageCache, SlotName, UserIndex);
	}
}

// 保存到硬盘
void UStorageSubsystem::SaveToDisk()
{
	if (StorageCache)
	{
		// Save to disk
		UGameplayStatics::AsyncSaveGameToSlot(StorageCache, SlotName, UserIndex);

		// Throttle save to Cloud
		// GetWorld()->GetTimerManager().SetTimer(SaveToCloudTimerHandle, this, &ThisClass::SaveToCloud, 5.f);
	}
}

// 保存到云端
void UStorageSubsystem::SaveToCloud()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		if (StorageCache)
		{
			TArray<uint8> FileContents;
			FMemoryWriter MemoryWriter(FileContents, true);
			FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
			Ar.ArIsSaveGame = false; // 无论属性设否设置了UPROPERTY(SaveGame)，都将进行序列化
			Ar.ArNoDelta = true;
			StorageCache->Serialize(Ar);

			EOSSubsystem->WriteFile(SlotName, FileContents); // 将本地存档保存到云端
		}
	}
}

// 保存到云端完成
void UStorageSubsystem::OnWriteFileComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnWriteFileComplete: %d"), bWasSuccessful);
}

// 应用设置
void UStorageSubsystem::ApplySetting()
{
	// 设置亮度
	GEngine->DisplayGamma = StorageCache->Brightness;

	// 设置音量
	AudioDevice = GEngine->GetActiveAudioDevice();
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AudioDevice && AssetSubsystem && AssetSubsystem->GetSoundMix())
	{
		AudioDevice->PushSoundMixModifier(AssetSubsystem->GetSoundMix());
		SetAudio(StorageCache->Volume);
	}
}

// 设置音量
void UStorageSubsystem::SetAudio(float Value)
{
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AudioDevice && AssetSubsystem && AssetSubsystem->GetSoundMix() && AssetSubsystem->GetMasterSound())
	{
		// UE_LOG(LogTemp, Warning, TEXT("SetAudio: %f"), Value); // TODO test
		AudioDevice->SetSoundMixClassOverride(AssetSubsystem->GetSoundMix(), AssetSubsystem->GetMasterSound(), Value, 1.f, 0.2f, true);
	}
}
