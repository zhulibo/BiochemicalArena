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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::NoTeam;
	void SetTeam(ETeam Team);

	UPROPERTY(ReplicatedUsing = OnRep_Defeat)
	int32 Defeat;

	void AddScore(float ScoreAmount);
	void AddDefeat(int32 DefeatAmount);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Team();
	virtual void OnRep_Score() override;
	UFUNCTION()
	void OnRep_Defeat();

public:
	FORCEINLINE float GetDefeat() const { return Defeat; }

};
