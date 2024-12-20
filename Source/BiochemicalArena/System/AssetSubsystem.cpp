#include "AssetSubsystem.h"

#include "AssetThread.h"
#include "DataAssetManager.h"
#include "Engine/StreamableManager.h"
#include "BiochemicalArena/System/Data/SystemSound.h"
#include "BiochemicalArena/System/Data/CommonAsset.h"
#include "BiochemicalArena/Characters/Data/InputBase.h"
#include "BiochemicalArena/Characters/Data/InputHuman.h"
#include "BiochemicalArena/Characters/Data/InputMutant.h"
#include "BiochemicalArena/Characters/Data/CharacterSound.h"
#include "BiochemicalArena/Characters/Data/InputSpectator.h"
#include "BiochemicalArena/Characters/Data/MutantCommon.h"

void UAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// AssetThread* Runnable = new AssetThread(this);
	// FRunnableThread* Thread = FRunnableThread::Create(Runnable, TEXT("AssetThread"));

	double StartTime = FPlatformTime::Seconds();
	
	SystemSoundId = GenerateId(USystemSound::StaticClass());
	CommonAssetId = GenerateId(UCommonAsset::StaticClass());
	InputBaseId = GenerateId(UInputBase::StaticClass());
	InputHumanId = GenerateId(UInputHuman::StaticClass());
	InputMutantId = GenerateId(UInputMutant::StaticClass());
	InputSpectatorId = GenerateId(UInputSpectator::StaticClass());

	CharacterSoundId = GenerateId(UCharacterSound::StaticClass());
	MutantCommonId = GenerateId(UMutantCommon::StaticClass());

	// Sync load asset
	FSoftObjectPath SystemSoundPath = UDataAssetManager::Get().GetPrimaryAssetPath(SystemSoundId);
	FSoftObjectPath CommonAssetPath = UDataAssetManager::Get().GetPrimaryAssetPath(CommonAssetId);
	FSoftObjectPath InputBasePath = UDataAssetManager::Get().GetPrimaryAssetPath(InputBaseId);
	FSoftObjectPath InputHumanPath = UDataAssetManager::Get().GetPrimaryAssetPath(InputHumanId);
	FSoftObjectPath InputMutantPath = UDataAssetManager::Get().GetPrimaryAssetPath(InputMutantId);
	FSoftObjectPath InputSpectatorPath = UDataAssetManager::Get().GetPrimaryAssetPath(InputSpectatorId);

	SystemSound = UDataAssetManager::Get().GetAsset(TSoftObjectPtr<USystemSound>(SystemSoundPath));
	CommonAsset = UDataAssetManager::Get().GetAsset(TSoftObjectPtr<UCommonAsset>(CommonAssetPath));
	InputBase = UDataAssetManager::Get().GetAsset(TSoftObjectPtr<UInputBase>(InputBasePath));
	InputHuman = UDataAssetManager::Get().GetAsset(TSoftObjectPtr<UInputHuman>(InputHumanPath));
	InputMutant = UDataAssetManager::Get().GetAsset(TSoftObjectPtr<UInputMutant>(InputMutantPath));
	InputSpectator = UDataAssetManager::Get().GetAsset(TSoftObjectPtr<UInputSpectator>(InputSpectatorPath));
	
	double EndTime = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Warning, TEXT("Sync load asset time: %f seconds"), EndTime - StartTime);

	// Async load asset
	TArray<FPrimaryAssetId> AssetIds;
	AssetIds.Emplace(CharacterSoundId);
	AssetIds.Emplace(MutantCommonId);
	UDataAssetManager::Get().LoadPrimaryAssets(AssetIds, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ThisClass::LoadCompleted));
}

FPrimaryAssetId UAssetSubsystem::GenerateId(UClass* Class)
{
	return FPrimaryAssetId(FPrimaryAssetType(Class->GetFName()), FName(Prefix + Class->GetFName().ToString()));
}

void UAssetSubsystem::LoadCompleted()
{
	CharacterSound = UDataAssetManager::Get().GetPrimaryAssetObject<UCharacterSound>(CharacterSoundId);
	MutantCommon = UDataAssetManager::Get().GetPrimaryAssetObject<UMutantCommon>(MutantCommonId);
}

void UAssetSubsystem::Test()
{
	UE_LOG(LogTemp, Warning, TEXT("Test"));
}
