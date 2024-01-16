#include "StorageSubsystem.h"
#include "AudioDevice.h"
#include "PlayerStorage.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
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
		// 保存存档至缓存
		PlayerStorageCache = PlayerStorage;

		// 保存存档至本地
		UGameplayStatics::SaveGameToSlot(PlayerStorageCache, SlotName, UserIndex);
	}
}

// 保存
void UStorageSubsystem::Save()
{
	if (PlayerStorageCache)
	{
		// Save to local immediately
		UGameplayStatics::AsyncSaveGameToSlot(PlayerStorageCache, SlotName, UserIndex);

		// Throttle save to Cloud
		// GetWorld()->GetTimerManager().SetTimer(WriteFileTimerHandle, this, &ThisClass::WriteFile, 5.f);
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

// 设置角色控制变量
void UStorageSubsystem::SetCharacterControlVariable()
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (BaseCharacter)
	{
		BaseCharacter->MouseSensitivityRate = MapSensitivity(PlayerStorageCache->MouseSensitivity);
		BaseCharacter->bMouseAimAssistSteering = PlayerStorageCache->MouseAimAssistSteering == "on";
		BaseCharacter->bMouseAimAssistSlowdown = PlayerStorageCache->MouseAimAssistSlowdown == "on";
		BaseCharacter->ControllerSensitivityRate = MapSensitivity(PlayerStorageCache->ControllerSensitivity);
		BaseCharacter->bControllerAimAssistSteering = PlayerStorageCache->ControllerAimAssistSteering == "on";
		BaseCharacter->bControllerAimAssistSlowdown = PlayerStorageCache->ControllerAimAssistSlowdown == "on";
	}
}

float UStorageSubsystem::MapSensitivity(float Value)
{
	if (Value < 50.f)
	{
		FVector2D InRange(1.f, 50.f);
		FVector2D OutRange(0.2f, 1.f);
		return FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
	}
	else if (Value > 50.f)
	{
		FVector2D InRange(50.f, 100.f);
		FVector2D OutRange(1.f, 5.f);
		return FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
	}
	else
	{
		return 1.f;
	}
}

// 初始化默认设置
void UStorageSubsystem::InitDefaultSetting()
{
	// 设置默认亮度
	GEngine->DisplayGamma = PlayerStorageCache->Brightness;

	// 加载声音资源
	SoundMix = LoadObject<USoundMix>(nullptr,TEXT("/Script/Engine.SoundMix'/Game/Assets/Sounds/SoundMix.SoundMix'"));
	MasterClass = LoadObject<USoundClass>(nullptr, TEXT("/Script/Engine.SoundClass'/Game/Assets/Sounds/Master.Master'"));

	AudioDevice = GEngine->GetActiveAudioDevice();
	if (AudioDevice && SoundMix)
	{
		AudioDevice->PushSoundMixModifier(SoundMix);
	}

	SetAudio(PlayerStorageCache->Volume);
}

// 设置音量
void UStorageSubsystem::SetAudio(float Value)
{
	if (AudioDevice && SoundMix && MasterClass)
	{
		FVector2D InRange(1.f, 100.f);
		FVector2D OutRange(0.01f, 1.f);
		Value =  FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
		AudioDevice->SetSoundMixClassOverride(SoundMix, MasterClass, Value, 1.f, 0.2f, true);
	}
}
