#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"

void ABasePlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerState, Defeat);
	DOREPLIFETIME(ABasePlayerState, Team);
}

void ABasePlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;
}

void ABasePlayerState::OnRep_Team()
{

}

void ABasePlayerState::AddScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
}

void ABasePlayerState::OnRep_Score()
{
	Super::OnRep_Score();
}

void ABasePlayerState::AddDefeat(int32 DefeatAmount)
{
	Defeat += DefeatAmount;
}

void ABasePlayerState::OnRep_Defeat()
{

}