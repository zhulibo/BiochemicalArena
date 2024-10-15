#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRoundStarted);
DECLARE_MULTICAST_DELEGATE(FOnRoundEnded);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAddKillLog, class ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState);

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API ABaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void AddToTeam(ABasePlayerState* BasePlayerState, ETeam Team);
	virtual void RemoveFromTeam(ABasePlayerState* BasePlayerState, ETeam Team);
	TArray<ABasePlayerState*> GetTeam(ETeam Team);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAddKillLog(ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState);

	virtual void OnRep_MatchState() override;
	FOnRoundStarted OnRoundStarted;
	FOnRoundEnded OnRoundEnded;
	FOnAddKillLog OnAddKillLog;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	UPROPERTY()
	class ABaseController* BaseController;

	virtual void HandleMatchHasStarted() override;
	virtual void HandleRoundHasEnded();

	UPROPERTY(ReplicatedUsing = OnRep_Team1)
	TArray<ABasePlayerState*> Team1;
	UPROPERTY(ReplicatedUsing = OnRep_Team2)
	TArray<ABasePlayerState*> Team2;
	UFUNCTION()
	virtual void OnRep_Team1() {}
	UFUNCTION()
	virtual void OnRep_Team2() {}

};
