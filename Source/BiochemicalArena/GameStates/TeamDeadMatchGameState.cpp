#include "TeamDeadMatchGameState.h"

#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/PlayerControllers/TeamDeadMatchController.h"
#include "Net/UnrealNetwork.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

ATeamDeadMatchGameState::ATeamDeadMatchGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATeamDeadMatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team1);
	DOREPLIFETIME(ThisClass, Team2);
	DOREPLIFETIME(ThisClass, Team1Score);
	DOREPLIFETIME(ThisClass, Team2Score);
}

void ATeamDeadMatchGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		WatchGameState();
	}
}

void ATeamDeadMatchGameState::WatchGameState()
{
	if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = Cast<ATeamDeadMatchMode>(GetWorld()->GetAuthGameMode());
	if (TeamDeadMatchMode == nullptr) return;

	// 监视比赛状态
	if (TeamDeadMatchMode->bWatchMatchState)
	{
		if (Team1.IsEmpty() || Team2.IsEmpty())
		{
			TeamDeadMatchMode->EndMatch();
		}
	}
}

float ATeamDeadMatchGameState::GetTeamScore(ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		return Team1Score;
	case ETeam::Team2:
		return Team2Score;
	default:
		return 0;
	}
}

void ATeamDeadMatchGameState::AddTeamScore(ETeam Team)
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

void ATeamDeadMatchGameState::OnRep_Team1Score()
{
	SetHUDTeamScore(Team1Score, ETeam::Team1);
}

void ATeamDeadMatchGameState::OnRep_Team2Score()
{
	SetHUDTeamScore(Team2Score, ETeam::Team2);
}

void ATeamDeadMatchGameState::SetHUDTeamScore(float Score, ETeam Team)
{
	if (TeamDeadMatchController == nullptr) TeamDeadMatchController = Cast<ATeamDeadMatchController>(GetWorld()->GetFirstPlayerController());
	if (TeamDeadMatchController)
	{
		TeamDeadMatchController->SetHUDTeamScore(Score, Team);
	}
}
