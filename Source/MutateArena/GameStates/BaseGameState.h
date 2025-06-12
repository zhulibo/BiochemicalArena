#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

enum class EMsgType : uint8;
DECLARE_MULTICAST_DELEGATE(FOnRoundStarted);
DECLARE_MULTICAST_DELEGATE(FOnRoundEnded);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAddKillLog, class ABasePlayerState* AttackerState, const FText& CauserName, ABasePlayerState* DamagedState);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnReceiveMsg, EMsgType MsgType, ETeam Team, const FString& PlayerName, const FString& Msg);

enum class ETeam : uint8;

UCLASS()
class MUTATEARENA_API ABaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void AddToPlayerStates(ABasePlayerState* BasePlayerState, ETeam Team);
	virtual void RemoveFromPlayerStates(ABasePlayerState* BasePlayerState, ETeam Team);
	TArray<ABasePlayerState*> GetPlayerStates(TOptional<ETeam> Team);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAddKillLog(ABasePlayerState* AttackerState, const FText& CauserName, ABasePlayerState* DamagedState);
	FOnAddKillLog OnAddKillLog;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSendMsg(EMsgType MsgType, ETeam Team, const FString& PlayerName, const FString& Msg = FString());
	FOnReceiveMsg OnReceiveMsg;

	virtual void OnRep_MatchState() override;
	FOnRoundStarted OnRoundStarted;
	FOnRoundEnded OnRoundEnded;

	UPROPERTY(Replicated)
	bool bCanSpectate = false;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	UPROPERTY()
	class ABaseController* BaseController;

	virtual void HandleMatchHasStarted() override;
	virtual void HandleRoundHasEnded();

	UPROPERTY(ReplicatedUsing = OnRep_Team1PlayerStates)
	TArray<ABasePlayerState*> Team1PlayerStates;
	UPROPERTY(ReplicatedUsing = OnRep_Team2PlayerStates)
	TArray<ABasePlayerState*> Team2PlayerStates;
	UFUNCTION()
	virtual void OnRep_Team1PlayerStates() {}
	UFUNCTION()
	virtual void OnRep_Team2PlayerStates() {}

	UPROPERTY()
	TArray<AActor*> AllEquipments;
	FTimerHandle GetAllEquipmentsTimerHandle;
	void SetAllEquipments();

public:
	FORCEINLINE TArray<AActor*> GetAllEquipments() const { return AllEquipments; }
	
};
