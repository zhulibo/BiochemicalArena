#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MAGameInstance.generated.h"

UCLASS()
class MUTATEARENA_API UMAGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
};
