#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ABaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddKillLog(class ABasePlayerState* AttackerState, const FString& WeaponName, ABasePlayerState* KilledState);

};
