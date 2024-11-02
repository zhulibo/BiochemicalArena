#include "BaseGameState.h"

#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Net/UnrealNetwork.h"

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team1);
	DOREPLIFETIME(ThisClass, Team2);
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

void ABaseGameState::AddToTeam(ABasePlayerState* BasePlayerState, ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		Team1.AddUnique(BasePlayerState);
		break;
	case ETeam::Team2:
		Team2.AddUnique(BasePlayerState);
		break;
	}
}

void ABaseGameState::RemoveFromTeam(ABasePlayerState* BasePlayerState, ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		Team1.Remove(BasePlayerState);
		break;
	case ETeam::Team2:
		Team2.Remove(BasePlayerState);
		break;
	}
}

TArray<ABasePlayerState*> ABaseGameState::GetTeam(ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		return Team1;
	case ETeam::Team2:
		return Team2;
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
