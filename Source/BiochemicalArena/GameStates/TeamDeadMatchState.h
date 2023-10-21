#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TeamDeadMatchState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ATeamDeadMatchState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AHumanState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<AHumanState*> TopScoringPlayers;

private:
	float TopScore = 0.f;

};
