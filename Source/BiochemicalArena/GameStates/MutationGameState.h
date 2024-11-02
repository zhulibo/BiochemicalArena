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
	
	void EndRoundIfAllBeKilledByMelee();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class AMutationMode* MutationMode;
	UPROPERTY()
	class AMutationController* MutationController;

	void WatchGameState();

	UPROPERTY(ReplicatedUsing = OnRep_DamageMul)
	float DamageMul = 1.f;
	void CalcDamageMul();
	UFUNCTION()
	void OnRep_DamageMul();

	virtual void OnRep_Team1() override;
	virtual void OnRep_Team2() override;

	void SetHUDTeamNum(int32 Team1Num, ETeam Team);

public:
	FORCEINLINE float GetDamageMul() const { return DamageMul; }

};
