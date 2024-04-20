#include "AssetThread.h"

#include "AssetSubsystem.h"

AssetThread::AssetThread(UAssetSubsystem* TempAssetSubsystem)
{
	AssetSubsystem = TempAssetSubsystem;
}

bool AssetThread::Init()
{
	return FRunnable::Init();
}

uint32 AssetThread::Run()
{
	if (AssetSubsystem)
	{
		AssetSubsystem->LoadCommonAsset();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, "AssetSubsystem nullptr!");
	}

	return 0;
}

void AssetThread::Stop()
{
	FRunnable::Stop();
}

void AssetThread::Exit()
{
	FRunnable::Exit();
}
