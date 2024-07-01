#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "MutationGameState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMutationGameState : public ABaseGameState
{
	GENERATED_BODY()

public:
	AMutationGameState();

	virtual void AddToTeam(ABasePlayerState* BasePlayerState, ETeam Team) override;
	virtual void RemoveFromTeam(ABasePlayerState* BasePlayerState, ETeam Team) override;

protected:
	virtual void Tick(float DeltaTime) override;

	void WatchGameState();

	UPROPERTY()
	class AMutationMode* MutationMode;
	UPROPERTY()
	class AMutationController* MutationController;

	virtual void OnRep_Team1() override;
	virtual void OnRep_Team2() override;

	void SetHUDTeamNum(int32 Team1Num, ETeam Team);

};
