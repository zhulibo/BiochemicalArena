#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TeamType.h"

ABasePlayerState::ABasePlayerState()
{
	Team = ETeam::NoTeam;
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, Defeat);
}

void ABasePlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerState::SetTeam(ETeam TemTeam)
{
	Team = TemTeam;
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
