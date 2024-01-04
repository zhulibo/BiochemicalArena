#include "BasePlayerState.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

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
