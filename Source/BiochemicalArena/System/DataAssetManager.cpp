#include "DataAssetManager.h"

UDataAssetManager::UDataAssetManager()
{
}

void UDataAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

UDataAssetManager& UDataAssetManager::Get()
{
	if (UDataAssetManager* Singleton = Cast<UDataAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogTemp, Error, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini. It must be set to UDataAssetManager!"));
	return *NewObject<UDataAssetManager>();
}

UObject* UDataAssetManager::SyncLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		if (IsInitialized())
		{
			return GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		return AssetPath.TryLoad();
	}

	return nullptr;
}

void UDataAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}
