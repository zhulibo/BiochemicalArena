#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "MAGameEngine.generated.h"

UCLASS()
class MUTATEARENA_API UMAGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:
	virtual void Init(IEngineLoop* InEngineLoop) override;
	
};