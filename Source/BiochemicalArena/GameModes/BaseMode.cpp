#include "BaseMode.h"

#include "EngineUtils.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "GameFramework/PlayerStart.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABaseMode::ABaseMode()
{
	bDelayedStart = true;
}

void ABaseMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartTime = GetWorld()->GetTimeSeconds();
}

void ABaseMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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

void ABaseMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABaseController* BaseController = Cast<ABaseController>(*It);
		if (BaseController)
		{
			BaseController->OnMatchStateSet(MatchState);
		}
	}
}

// 按tag寻找一个随机的出生点
AActor* ABaseMode::FindRandomPlayerStart(const FName& PlayerStartTag)
{
	APlayerStart* FoundPlayerStart = nullptr;

	if (PlayerStartTag.IsValid())
	{
		TArray<APlayerStart*> StartPoints;

		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			APlayerStart* StartPoint = *It;
			if (StartPoint && StartPoint->PlayerStartTag == PlayerStartTag)
			{
				StartPoints.Add(StartPoint);
			}
		}

		if (StartPoints.Num() > 0)
		{
			FoundPlayerStart = StartPoints[FMath::RandRange(0, StartPoints.Num() - 1)];
		}
	}

	return FoundPlayerStart;
}
