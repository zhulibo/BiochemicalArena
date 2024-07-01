#include "TeamDeadMatchController.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchGameState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/HUD/Mutation.h"
#include "BiochemicalArena/UI/HUD/TeamDeadMatch.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ATeamDeadMatchController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
}

void ATeamDeadMatchController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		RequestServerMatchInfo();

		if (HUDContainer)
		{
			HUDContainer->TeamDeadMatch->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ATeamDeadMatchController::RequestServerMatchInfo_Implementation()
{
	if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = Cast<ATeamDeadMatchMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (TeamDeadMatchMode)
	{
		ReturnServerMatchInfo(
			TeamDeadMatchMode->GetMatchState(),
			TeamDeadMatchMode->LevelStartTime,
			TeamDeadMatchMode->GetWarmupTime(),
			TeamDeadMatchMode->GetMatchTime(),
			TeamDeadMatchMode->GetCooldownTime()
		);
	}
}

void ATeamDeadMatchController::ReturnServerMatchInfo_Implementation(
	FName TempMatchState,
	float TempLevelStartTime,
	float TempWarmupTime,
	float TempMatchTime,
	float TempCooldownTime
){
	MatchState = TempMatchState;
	HandleMatchStateChange();

	LevelStartTime = TempLevelStartTime;
	WarmupTime = TempWarmupTime;
	MatchTime = TempMatchTime;
	CooldownTime = TempCooldownTime;
}

void ATeamDeadMatchController::OnMatchStateSet(FName TempMatchState)
{
	MatchState = TempMatchState;
	HandleMatchStateChange();
}

void ATeamDeadMatchController::OnRep_MatchState()
{
	HandleMatchStateChange();
}

void ATeamDeadMatchController::HandleMatchStateChange()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::WaitingPostMatch)
	{
		HandleMatchHasEnded();
	}
}

void ATeamDeadMatchController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = LevelStartTime + WarmupTime - GetServerTime();
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = LevelStartTime + WarmupTime + MatchTime - GetServerTime();
	}
	else if (MatchState == MatchState::WaitingPostMatch)
	{
		TimeLeft = LevelStartTime + CooldownTime + WarmupTime + MatchTime - GetServerTime();
	}
	int32 SecondsLeft = FMath::RoundToInt(TimeLeft);

	// 当前时间秒数变化时更新HUD
	if (SecondsLeft != CountdownSeconds)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDWarmupCountdown(SecondsLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(SecondsLeft);
		}
		else if (MatchState == MatchState::WaitingPostMatch)
		{
			UE_LOG(LogTemp, Warning, TEXT("Match has ended"));
			// ClientTravel("/Game/Maps/UI_Menu", ETravelType::TRAVEL_Absolute);
		}

		// 记录当前时间秒数
		CountdownSeconds = SecondsLeft;
	}
}

void ATeamDeadMatchController::SetHUDMatchCountdown(int32 CountdownTime)
{
	if (HUDContainer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HUDContainer->TeamDeadMatch->MatchCountdown->SetText(FText::FromString(CountdownText));
	}
}

void ATeamDeadMatchController::InitHUD()
{
	if (TeamDeadMatchGameState == nullptr) TeamDeadMatchGameState = Cast<ATeamDeadMatchGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetPawn());

	if (TeamDeadMatchGameState && HumanCharacter)
	{
		SetHUDHealth(HumanCharacter->GetMaxHealth());
		SetHUDTeamScore(TeamDeadMatchGameState->GetTeamScore(ETeam::Team1), ETeam::Team1);
		SetHUDTeamScore(TeamDeadMatchGameState->GetTeamScore(ETeam::Team2), ETeam::Team2);

		bNeedInitHUD = false;
	}
}

void ATeamDeadMatchController::SetHUDHealth(float Health)
{
	if (HUDContainer)
	{
		FNumberFormattingOptions Opts;
		Opts.RoundingMode = ERoundingMode::ToPositiveInfinity; // 向上取整
		Opts.SetUseGrouping(false); // 不使用千位分隔符
		HUDContainer->TeamDeadMatch->Health->SetText(FText::AsNumber(Health, &Opts));
	}
}

void ATeamDeadMatchController::SetHUDAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->Ammo->SetText(FText::AsNumber(Ammo));
	}
}

void ATeamDeadMatchController::SetHUDCarriedAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->CarriedAmmo->SetText(FText::AsNumber(Ammo));
	}
}

void ATeamDeadMatchController::SetHUDTeamScore(float Score, ETeam Team)
{
	if (HUDContainer)
	{
		switch (Team)
		{
		case ETeam::Team1:
			HUDContainer->TeamDeadMatch->Team1Score->SetText(FText::AsNumber(Score));
			break;
		case ETeam::Team2:
			HUDContainer->TeamDeadMatch->Team2Score->SetText(FText::AsNumber(Score));
			break;
		}
	}
}
