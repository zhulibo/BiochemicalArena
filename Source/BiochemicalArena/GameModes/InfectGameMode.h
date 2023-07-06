#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InfectGameMode.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AInfectGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() override;
};
