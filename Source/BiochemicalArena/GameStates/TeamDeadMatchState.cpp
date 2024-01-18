#include "TeamDeadMatchState.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Net/UnrealNetwork.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

void ATeamDeadMatchState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team1);
	DOREPLIFETIME(ThisClass, Team2);
	DOREPLIFETIME(ThisClass, Team1Score);
	DOREPLIFETIME(ThisClass, Team2Score);
}

float ATeamDeadMatchState::GetTeamScore(ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		return Team1Score;
	case ETeam::Team2:
		return Team2Score;
	case ETeam::NoTeam:
		return 0;
	default:
		return 0;
	}
}

void ATeamDeadMatchState::AddTeamScore(ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		Team1Score++;
		SetHUDTeamScore(Team1Score, Team);
		break;
	case ETeam::Team2:
		Team2Score++;
		SetHUDTeamScore(Team2Score, Team);
		break;
	}
}

TArray<AHumanState*> ATeamDeadMatchState::GetTeam(ETeam Team)
{
	if (Team == ETeam::Team1)
	{
		return Team1;
	}
	return Team2; // HACK 避免调用GetTeam时需要判断返回值是否为nullptr
}

void ATeamDeadMatchState::AddToTeam(AHumanState* AHumanState, ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		Team1.AddUnique(AHumanState);
		break;
	case ETeam::Team2:
		Team2.AddUnique(AHumanState);
		break;
	}
}

void ATeamDeadMatchState::OnRep_Team1Score()
{
	SetHUDTeamScore(Team1Score, ETeam::Team1);
}

void ATeamDeadMatchState::OnRep_Team2Score()
{
	SetHUDTeamScore(Team2Score, ETeam::Team2);
}

void ATeamDeadMatchState::SetHUDTeamScore(float Score, ETeam Team)
{
	if (HumanController == nullptr) HumanController = Cast<AHumanController>(GetWorld()->GetFirstPlayerController());
	if (HumanController)
	{
		HumanController->SetHUDTeamScore(Score, Team);
	}
}
