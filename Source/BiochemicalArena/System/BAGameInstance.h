#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BAGameInstance.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBAGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
};
