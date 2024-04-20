#pragma once

#include "CoreMinimal.h"

class AssetThread : public FRunnable
{

public:
	AssetThread(class UAssetSubsystem* TempAssetSubsystem);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	UAssetSubsystem* AssetSubsystem;

};
