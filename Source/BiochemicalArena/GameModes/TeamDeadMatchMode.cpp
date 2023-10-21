#include "TeamDeadMatchMode.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ATeamDeadMatchMode::ATeamDeadMatchMode()
{
	bDelayedStart = true;
}

void ATeamDeadMatchMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartTime = GetWorld()->GetTimeSeconds();
}

void ATeamDeadMatchMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = LevelStartTime + WarmupTime - GetWorld()->GetTimeSeconds();
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = LevelStartTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds();
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = LevelStartTime + CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds();
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ATeamDeadMatchMode::PlayerEliminated(AHumanCharacter* ElimmedCharacter, AHumanController* VictimController,
	AHumanController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;

	AHumanState* AttackerState = AttackerController ? Cast<AHumanState>(AttackerController->PlayerState) : nullptr;
	AHumanState* VictimState = VictimController ? Cast<AHumanState>(VictimController->PlayerState) : nullptr;
	ATeamDeadMatchState* TeamDeadMatchState = GetGameState<ATeamDeadMatchState>();

	if (AttackerState && AttackerState != VictimState && TeamDeadMatchState)
	{
		AttackerState->AddToScore(1.f);
		TeamDeadMatchState->UpdateTopScore(AttackerState);
	}
	if (VictimState)
	{
		VictimState->AddToDefeats(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ATeamDeadMatchMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

void ATeamDeadMatchMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AHumanController* HumanController = Cast<AHumanController>(*It);
		if (HumanController)
		{
			HumanController->OnMatchStateSet(MatchState);
		}
	}
}