#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "TeamDeadMatchPlayerState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ATeamDeadMatchPlayerState : public ABasePlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

};
