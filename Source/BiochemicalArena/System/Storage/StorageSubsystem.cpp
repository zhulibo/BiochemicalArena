#include "StorageSubsystem.h"

#include "BiochemicalArena/System/AssetSubsystem.h"
#include "AudioDevice.h"
#include "SaveGameSetting.h"
#include "BiochemicalArena/UI/Setting/TabAudio.h"
#include "BiochemicalArena/System/Data/SystemSound.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UStorageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	// if (EOSSubsystem)
	// {
	// 	EOSSubsystem->OnWriteFileComplete.AddUObject(this, &ThisClass::OnWriteFileComplete);
	// }

	// 如果本地存档存在，则放到缓存里
	if (UGameplayStatics::DoesSaveGameExist(SettingSlotName, UserIndex))
	{
		if (USaveGameSetting* SaveGameSetting = Cast<USaveGameSetting>(UGameplayStatics::LoadGameFromSlot(SettingSlotName, UserIndex)))
		{
			CacheSetting = SaveGameSetting;

			// 应用设置
			ApplySetting();
		}
		else
		{
			CreateSaveGameSetting();
		}
	}
	// 如果本地存档不存在，则创建本地存档
	else
	{
		CreateSaveGameSetting();
	}
}

// 创建玩家存档
void UStorageSubsystem::CreateSaveGameSetting()
{
	USaveGameSetting* SaveGameSetting = Cast<USaveGameSetting>(UGameplayStatics::CreateSaveGameObject(USaveGameSetting::StaticClass()));
	if (SaveGameSetting)
	{
		// 保存存档至缓存
		CacheSetting = SaveGameSetting;

		// 应用设置
		ApplySetting();

		// 保存存档至本地
		UGameplayStatics::SaveGameToSlot(CacheSetting, SettingSlotName, UserIndex);
	}
}

// 保存
void UStorageSubsystem::Save()
{
	if (CacheSetting)
	{
		// Save to disk
		UGameplayStatics::AsyncSaveGameToSlot(CacheSetting, SettingSlotName, UserIndex);

		// Throttle save to Cloud
		GetWorld()->GetTimerManager().SetTimer(SaveToCloudTimerHandle, this, &ThisClass::SaveToCloud, 5.f);
	}
}

// 保存到云端
void UStorageSubsystem::SaveToCloud()
{
	// if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	// if (EOSSubsystem && StorageCache)
	// {
	// 	TArray<uint8> FileContents;
	// 	FMemoryWriter MemoryWriter(FileContents, true);
	// 	FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
	// 	Ar.ArIsSaveGame = false; // 无论属性设否设置了UPROPERTY(SaveGame)，都将进行序列化
	// 	Ar.ArNoDelta = true;
	// 	StorageCache->Serialize(Ar);
	//
	// 	// 将本地存档保存到云端
	// 	EOSSubsystem->WriteFile(SlotName, FileContents);
	// }
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
	GEngine->DisplayGamma = CacheSetting->Brightness;

	// 设置音量
	AudioDevice = GEngine->GetActiveAudioDevice();
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AudioDevice && AssetSubsystem && AssetSubsystem->SystemSound)
	{
		AudioDevice->PushSoundMixModifier(AssetSubsystem->SystemSound->SoundMix);

		SetAudio(CacheSetting->MasterVolume, ESoundClassType::Master);
		SetAudio(CacheSetting->EffectsVolume, ESoundClassType::Effects);
		SetAudio(CacheSetting->MusicVolume, ESoundClassType::Music);
		SetAudio(CacheSetting->DialogueVolume, ESoundClassType::Dialogue);
	}
}

// 设置音量
void UStorageSubsystem::SetAudio(float Value, ESoundClassType SoundClassType)
{
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AudioDevice && AssetSubsystem && AssetSubsystem->SystemSound)
	{
		// UE_LOG(LogTemp, Warning, TEXT("SetAudio: %f"), Value); // TODO test
		USoundClass* SoundClass;
		switch (SoundClassType)
		{
		case ESoundClassType::Master:
			SoundClass = AssetSubsystem->SystemSound->MasterSound;
			break;
		case ESoundClassType::Effects:
			SoundClass = AssetSubsystem->SystemSound->EffectsSound;
			break;
		case ESoundClassType::Music:
			SoundClass = AssetSubsystem->SystemSound->MusicSound;
			break;
		case ESoundClassType::Dialogue:
			SoundClass = AssetSubsystem->SystemSound->DialogueSound;
			break;
		default:
			SoundClass = AssetSubsystem->SystemSound->MasterSound;
			break;
		}

		AudioDevice->SetSoundMixClassOverride(
			AssetSubsystem->SystemSound->SoundMix,
			SoundClass,
			Value,
			1.f,
			0.2f,
			true
		);
	}
}
