#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "TeamDeadMatchState.generated.h"

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API ATeamDeadMatchState : public ABaseGameState
{
	GENERATED_BODY()

public:
	float GetTeamScore(ETeam Team);
	void AddTeamScore(ETeam Team);

	TArray<class AHumanState*> GetTeam(ETeam Team);
	void AddToTeam(AHumanState* AHumanState, ETeam Team);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TArray<AHumanState*> Team1;
	UPROPERTY(Replicated)
	TArray<AHumanState*> Team2;

	UPROPERTY(ReplicatedUsing = OnRep_Team1Score)
	float Team1Score = 0.f;
	UPROPERTY(ReplicatedUsing = OnRep_Team2Score)
	float Team2Score = 0.f;

	UFUNCTION()
	void OnRep_Team1Score();
	UFUNCTION()
	void OnRep_Team2Score();

	UPROPERTY()
	class AHumanController* HumanController;
	void SetHUDTeamScore(float Score, ETeam Team);

};
