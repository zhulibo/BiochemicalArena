#include "MutationGameState.h"

#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

AMutationGameState::AMutationGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMutationGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		WatchGameState();
	}
}

void AMutationGameState::WatchGameState()
{
	if (MutationMode == nullptr) MutationMode = Cast<AMutationMode>(GetWorld()->GetAuthGameMode());
	if (MutationMode == nullptr) return;

	// 监视比赛状态
	if (MutationMode->bWatchMatchState)
	{
		if (Team1.Num() + Team2.Num() < 2)
		{
			MutationMode->EndMatch();
		}
	}

	// 监视对局状态
	if (MutationMode->bWatchRoundState)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Team1.Num(): %d, Team2.Num(): %d"), Team1.Num(), Team2.Num());
		if (Team1.Num() == 0 || Team2.Num() == 0)
		{
			MutationMode->EndRound();
		}
	}
}

void AMutationGameState::AddToTeam(ABasePlayerState* BasePlayerState, ETeam Team)
{
	Super::AddToTeam(BasePlayerState, Team);

	SetHUDTeamNum(GetTeam(Team).Num(), Team);
}

void AMutationGameState::RemoveFromTeam(ABasePlayerState* BasePlayerState, ETeam Team)
{
	Super::RemoveFromTeam(BasePlayerState, Team);

	SetHUDTeamNum(GetTeam(Team).Num(), Team);
}

void AMutationGameState::OnRep_Team1()
{
	Super::OnRep_Team1();

	SetHUDTeamNum(Team1.Num(), ETeam::Team1);
}

void AMutationGameState::OnRep_Team2()
{
	Super::OnRep_Team2();

	SetHUDTeamNum(Team2.Num(), ETeam::Team2);
}

void AMutationGameState::SetHUDTeamNum(int32 TeamNum, ETeam Team)
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetWorld()->GetFirstPlayerController());
	if (MutationController)
	{
		MutationController->SetHUDTeamNum(TeamNum, Team);
	}
}
