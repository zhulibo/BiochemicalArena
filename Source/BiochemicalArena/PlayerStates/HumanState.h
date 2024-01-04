#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "HumanState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanState : public ABasePlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

};
