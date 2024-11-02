#include "AssetSubsystem.h"

#include "AssetThread.h"
#include "BiochemicalArena/System/Data/SystemSound.h"
#include "BiochemicalArena/System/Data/CommonAsset.h"
#include "BiochemicalArena/Characters/Data/CharacterSound.h"
#include "BiochemicalArena/Characters/Data/MutantCommon.h"

void UAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 开启新线程 调用LoadCommonAsset
	// AssetThread* Runnable = new AssetThread(this);
	// FRunnableThread* Thread = FRunnableThread::Create(Runnable, TEXT("AssetThread"));

	// double StartTime = FPlatformTime::Seconds();

	SystemSound = LoadObject<USystemSound>(nullptr, TEXT("/Script/BiochemicalArena.SystemSound'/Game/Audio/DA_SystemSound.DA_SystemSound'"));
	CommonAsset = LoadObject<UCommonAsset>(nullptr, TEXT("/Script/BiochemicalArena.CommonAsset'/Game/System/Data/DA_CommonAsset.DA_CommonAsset'"));
	CharacterSound = LoadObject<UCharacterSound>(nullptr, TEXT("/Script/BiochemicalArena.CharacterSound'/Game/Audio/Footsteps/DA_CharacterSound.DA_CharacterSound'"));
	MutantCommon = LoadObject<UMutantCommon>(nullptr, TEXT("/Script/BiochemicalArena.MutantCommon'/Game/Characters/Data/DA_MutantCommon.DA_MutantCommon'"));

	// double EndTime = FPlatformTime::Seconds();
	// UE_LOG(LogTemp, Warning, TEXT("LoadCommonAsset time: %f seconds"), EndTime - StartTime);
}

void UAssetSubsystem::Test()
{
}
