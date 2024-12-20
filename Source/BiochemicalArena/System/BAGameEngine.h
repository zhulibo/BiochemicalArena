#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "BAGameEngine.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBAGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:
	virtual void Init(IEngineLoop* InEngineLoop) override;
	
};