#include "BaseGameState.h"

#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Net/UnrealNetwork.h"

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team1PlayerStates);
	DOREPLIFETIME(ThisClass, Team2PlayerStates);
	DOREPLIFETIME(ThisClass, bCanSpectate);
}

void ABaseGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	if (MatchState == MatchState::PostRound)
	{
		HandleRoundHasEnded();
	}
}

void ABaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	OnRoundStarted.Broadcast();
}

void ABaseGameState::HandleRoundHasEnded()
{
	OnRoundEnded.Broadcast();
}

void ABaseGameState::AddToPlayerStates(ABasePlayerState* BasePlayerState, ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		Team1PlayerStates.AddUnique(BasePlayerState);
		break;
	case ETeam::Team2:
		Team2PlayerStates.AddUnique(BasePlayerState);
		break;
	}
}

void ABaseGameState::RemoveFromPlayerStates(ABasePlayerState* BasePlayerState, ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		Team1PlayerStates.Remove(BasePlayerState);
		break;
	case ETeam::Team2:
		Team2PlayerStates.Remove(BasePlayerState);
		break;
	}
}

TArray<ABasePlayerState*> ABaseGameState::GetPlayerStates(ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		return Team1PlayerStates;
	case ETeam::Team2:
		return Team2PlayerStates;
	default:
		return TArray<ABasePlayerState*>();
	}
}

void ABaseGameState::MulticastAddKillLog_Implementation(ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState)
{
	OnAddKillLog.Broadcast(AttackerState, CauserName, DamagedState);
}

void ABaseGameState::MulticastSendMsg_Implementation(const EMsgType MsgType, const ETeam Team, const FString& PlayerName, const FString& Msg)
{
	OnReceiveMsg.Broadcast(MsgType, Team, PlayerName, Msg);
}
