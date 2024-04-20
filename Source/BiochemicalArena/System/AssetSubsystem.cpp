#include "AssetSubsystem.h"

#include "AudioDevice.h"
#include "BiochemicalArena/Characters/CharacterType.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"
#include "AssetThread.h"
#include "Sound/SoundCue.h"

void UAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 开启新线程 调用LoadCommonAsset
	// AssetThread* Runnable = new AssetThread(this);
	// FRunnableThread* Thread = FRunnableThread::Create(Runnable, TEXT("AssetThread"));

	double StartTime = FPlatformTime::Seconds();
	LoadSystemSoundAsset();
	LoadDataTableAsset();
	LoadCommonAsset();
	double EndTime = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Warning, TEXT("LoadCommonAsset time: %f seconds"), EndTime - StartTime);
}

// 加载系统音效资源
void UAssetSubsystem::LoadSystemSoundAsset()
{
	SoundMix = LoadObject<USoundMix>(nullptr,TEXT("/Script/Engine.SoundMix'/Game/Audio/SoundMix.SoundMix'"));
	MasterSound = LoadObject<USoundClass>(nullptr, TEXT("/Script/Engine.SoundClass'/Game/Audio/MasterSound.MasterSound'"));
}

USoundMix* UAssetSubsystem::GetSoundMix()
{
	if (SoundMix)
	{
		return SoundMix;
	}
	else
	{
		LoadSystemSoundAsset();
		return SoundMix;
	}
}

USoundClass* UAssetSubsystem::GetMasterSound()
{
	if (MasterSound)
	{
		return MasterSound;
	}
	else
	{
		LoadSystemSoundAsset();
		return MasterSound;
	}
}

// 加载数据表资源
void UAssetSubsystem::LoadDataTableAsset()
{
	HumanCharacterDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/Characters/Data/DT_HumanCharacter.DT_HumanCharacter'"));
	if (HumanCharacterDataTable)
	{
		HumanCharacterDataTable->GetAllRows<FHumanCharacterData>("", HumanCharacterDataRows);
	}

	EquipmentDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/Equipments/Data/DT_Equipment.DT_Equipment'"));
	if (EquipmentDataTable)
	{
		EquipmentDataTable->GetAllRows<FEquipmentData>("", EquipmentDataRows);
	}
}

TArray<FHumanCharacterData*> UAssetSubsystem::GetHumanCharacterDataRows()
{
	if (HumanCharacterDataTable)
	{
		return HumanCharacterDataRows;
	}
	else
	{
		LoadDataTableAsset();
		return HumanCharacterDataRows;
	}
}

TArray<FEquipmentData*> UAssetSubsystem::GetEquipmentDataRows()
{
	if (EquipmentDataTable)
	{
		return EquipmentDataRows;
	}
	else
	{
		LoadDataTableAsset();
		return EquipmentDataRows;
	}
}

// 加载公共资源
void UAssetSubsystem::LoadCommonAsset()
{
	ShellSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Shells/Shells_Cue.Shells_Cue'"));

	EquipSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	ClickSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));

	MetalSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	WaterSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	GrassSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	MudSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	CommonSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Common_Cue.Footsteps_Common_Cue'"));

	OuchSound1 = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	OuchSound2 = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	OuchSound3 = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Audio/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
}
