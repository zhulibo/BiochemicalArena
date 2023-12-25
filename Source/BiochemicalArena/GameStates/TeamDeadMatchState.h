#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerStates/Team.h"
#include "TeamDeadMatchState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ATeamDeadMatchState : public ABaseGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TArray<AHumanState*> Team1;
	UPROPERTY(Replicated)
	TArray<AHumanState*> Team2;

	UPROPERTY(ReplicatedUsing = OnRep_Team1Score)
	float Team1Score = 0.f;
	UPROPERTY(ReplicatedUsing = OnRep_Team2Score)
	float Team2Score = 0.f;

	void AddTeamScore(ETeam Team);

protected:
	UFUNCTION()
	void OnRep_Team1Score();
	UFUNCTION()
	void OnRep_Team2Score();

	void SetHUDTeamScore(float Score, int32 Team);

};
