#pragma once

#include "CoreMinimal.h"
#include "Team.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void SetTeam(ETeam Team);

	void AddScore(float ScoreAmount);
	void AddDefeat(int32 DefeatAmount);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, Category = "Player")
	ETeam Team = ETeam::NoTeam;
	UFUNCTION()
	void OnRep_Team();

	virtual void OnRep_Score() override;

	UPROPERTY(ReplicatedUsing = OnRep_Defeat)
	int32 Defeat;
	UFUNCTION()
	void OnRep_Defeat();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE float GetDefeat() const { return Defeat; }

};
