#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "MeleePlayerState.generated.h"

UCLASS()
class MUTATEARENA_API AMeleePlayerState : public ABasePlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

};
