#include "AssetThread.h"

#include "AssetSubsystem.h"
#include "BiochemicalArena/BiochemicalArena.h"

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
		AssetSubsystem->Test();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("AssetSubsystem nullptr!"), false);
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
