#include "TeamDeadMatchState.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Net/UnrealNetwork.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"

void ATeamDeadMatchState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamDeadMatchState, Team1);
	DOREPLIFETIME(ATeamDeadMatchState, Team2);
	DOREPLIFETIME(ATeamDeadMatchState, Team1Score);
	DOREPLIFETIME(ATeamDeadMatchState, Team2Score);
}

void ATeamDeadMatchState::AddTeamScore(ETeam Team)
{
	if (Team == ETeam::Team1)
	{
		Team1Score++;
		SetHUDTeamScore(Team1Score, 1);
	}
	else if (Team == ETeam::Team2)
	{
		Team2Score++;
		SetHUDTeamScore(Team2Score, 2);
	}
}

void ATeamDeadMatchState::SetHUDTeamScore(float Score, int32 Team)
{
	AHumanController* HumanController = Cast<AHumanController>(GetWorld()->GetFirstPlayerController());
	if (HumanController)
	{
		HumanController->SetHUDTeamScore(Score, Team);
	}
}

void ATeamDeadMatchState::OnRep_Team1Score()
{
	SetHUDTeamScore(Team1Score, 1);
}

void ATeamDeadMatchState::OnRep_Team2Score()
{
	SetHUDTeamScore(Team2Score, 2);
}
