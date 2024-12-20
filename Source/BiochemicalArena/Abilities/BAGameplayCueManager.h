#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "BAGameplayCueManager.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBAGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:
	static UBAGameplayCueManager* Get();

protected:
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;

};
