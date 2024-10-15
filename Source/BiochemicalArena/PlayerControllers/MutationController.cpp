#include "MutationController.h"

#include "AbilitySystemComponent.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/PlayerStates/MutationPlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/UI/HUD/Mutation/MutationMutant.h"
#include "BiochemicalArena/UI/HUD/RadialMenu/RadialMenuContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AMutationController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, CurrentRound);
}

void AMutationController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		RequestServerMatchInfo();
	}
}

void AMutationController::RequestServerMatchInfo_Implementation()
{
	if (MutationMode == nullptr) MutationMode = Cast<AMutationMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MutationMode)
	{
		ReturnServerMatchInfo(
			MutationMode->GetMatchState(),
			MutationMode->GetTotalRound(),
			MutationMode->LevelStartTime,
			MutationMode->GetWarmupTime(),
			MutationMode->GetRoundTime(),
			MutationMode->GetMutateTime(),
			MutationMode->GetPostRoundTime(),
			MutationMode->GetCooldownTime()
		);
	}
}

void AMutationController::ReturnServerMatchInfo_Implementation(
	FName TempMatchState,
	int32 TempTotalRound,
	float TempLevelStartTime,
	float TempWarmupTime,
	float TempRoundTime,
	float TempMutateTime,
	float TempPostRoundTime,
	float TempCooldownTime
){
	MatchState = TempMatchState;
	HandleMatchStateChange();

	TotalRound = TempTotalRound;
	SetHUDTotalRound();

	LevelStartTime = TempLevelStartTime;
	WarmupTime = TempWarmupTime;
	RoundTime = TempRoundTime;
	MutateTime = TempMutateTime;
	PostRoundTime = TempPostRoundTime;
	CooldownTime = TempCooldownTime;
}

void AMutationController::OnMatchStateSet(FName TempMatchState, int32 TempCurrentRound)
{
	MatchState = TempMatchState;
	HandleMatchStateChange();

	CurrentRound = TempCurrentRound;
	if (IsLocalController())
	{
		SetHUDCurrentRound();
	}
}

void AMutationController::OnRep_MatchState()
{
	HandleMatchStateChange();
}

void AMutationController::HandleMatchStateChange()
{
	if (MatchState == MatchState::InProgress)
	{
		RoundStartTime = GetServerTime();

		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::PostRound)
	{
		RoundEndTime = GetServerTime();

		HandleRoundHasEnded();
	}
	else if (MatchState == MatchState::WaitingPostMatch)
	{
		HandleMatchHasEnded();
	}
}

void AMutationController::HandleRoundHasEnded()
{
	ChangeAnnouncement.Broadcast(FText::FromString("Round end"));
}

void AMutationController::OnRep_CurrentRound()
{
	SetHUDCurrentRound();
}

void AMutationController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = LevelStartTime + WarmupTime - GetServerTime();
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = RoundStartTime + RoundTime - GetServerTime();
	}
	else if (MatchState == MatchState::PostRound)
	{
		TimeLeft = RoundEndTime + PostRoundTime - GetServerTime();
	}
	else if (MatchState == MatchState::WaitingPostMatch)
	{
		TimeLeft = RoundEndTime + PostRoundTime + CooldownTime - GetServerTime();
	}
	int32 SecondsLeft = FMath::RoundToInt(TimeLeft);

	// 当前时间秒数变化时更新HUD
	if (SecondsLeft != CountdownSeconds)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDWarmupCountdown(SecondsLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			OnMatchCountdownChange.Broadcast(SecondsLeft);

			// 突变倒计时
			int32 MutateCountdown = FMath::RoundToInt(MutateTime + TimeLeft - RoundTime);
			if (MutateCountdown > 0)
			{
				ChangeAnnouncement.Broadcast(FText::FromString(FString::Printf(TEXT("%d"), MutateCountdown)));
				// TODO 声音
			}
			else if (MutateCountdown == 0)
			{
				ChangeAnnouncement.Broadcast(FText());
			}
		}
		else if (MatchState == MatchState::WaitingPostMatch)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Match has ended"));
			// ClientTravel("/Game/Maps/UI_Menu", ETravelType::TRAVEL_Absolute);
		}

		// 记录当前时间秒数
		CountdownSeconds = SecondsLeft;
	}
}

void AMutationController::InitHUD()
{
	Super::InitHUD();

	if (MutationPlayerState == nullptr) MutationPlayerState = Cast<AMutationPlayerState>(PlayerState);
	if (MutationPlayerState)
	{
		if (MutationPlayerState->GetTeam() == ETeam::Team1)
		{
			InitHumanHUD();
		}
		else if(MutationPlayerState->GetTeam() == ETeam::Team2)
		{
			InitMutantHUD();
		}
	}
}

void AMutationController::InitHumanHUD()
{
	if (MutationGameState == nullptr) MutationGameState = Cast<AMutationGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (BaseCharacter == nullptr) BaseCharacter = Cast<ABaseCharacter>(GetPawn());

	if (MutationGameState && BaseCharacter)
	{
		SetHUDHealth(BaseCharacter->GetMaxHealth());
		SetHUDCurrentRound();
		SetHUDTeamNum(MutationGameState->GetTeam(ETeam::Team1).Num(), ETeam::Team1);
		SetHUDTeamNum(MutationGameState->GetTeam(ETeam::Team2).Num(), ETeam::Team2);
		SetHUDDamageMul(MutationGameState->GetDamageMul());
	}
}

void AMutationController::InitMutantHUD()
{
	if (MutationGameState == nullptr) MutationGameState = Cast<AMutationGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (BaseCharacter == nullptr) BaseCharacter = Cast<ABaseCharacter>(GetPawn());
	if (MutationPlayerState == nullptr) MutationPlayerState = Cast<AMutationPlayerState>(PlayerState);

	if (MutationGameState && BaseCharacter && MutationPlayerState)
	{
		SetHUDHealth(BaseCharacter->GetMaxHealth());
		SetHUDCurrentRound();
		SetHUDTeamNum(MutationGameState->GetTeam(ETeam::Team1).Num(), ETeam::Team1);
		SetHUDTeamNum(MutationGameState->GetTeam(ETeam::Team2).Num(), ETeam::Team2);
		SetHUDRage(MutationPlayerState->GetRage());

		if (MutationPlayerState->GetAbilitySystemComponent())
		{
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TAG_Mutant_SKILL_CD);
			int32 TagCount = MutationPlayerState->GetAbilitySystemComponent()->GetTagCount(Tag);
			ShowHUDSkill(TagCount == 0.f && MutationPlayerState->GetCharacterLevel() >= 2.f);
		}
	}
}

void AMutationController::SetHUDHealth(float Health)
{
	if (MutationPlayerState == nullptr) MutationPlayerState = Cast<AMutationPlayerState>(PlayerState);
	if (MutationPlayerState)
	{
		if (MutationPlayerState->GetTeam() == ETeam::Team1)
		{
			OnHumanHealthChange.Broadcast(Health);
		}
		else if(MutationPlayerState->GetTeam() == ETeam::Team2)
		{
			OnMutantHealthChange.Broadcast(Health);
		}
	}
}

void AMutationController::SetHUDTeamNum(int32 TeamNum, ETeam Team)
{
	OnTeamNumChange.Broadcast(TeamNum, Team);
}

void AMutationController::SetHUDCurrentRound()
{
	OnCurRoundChange.Broadcast(CurrentRound);
}

void AMutationController::SetHUDTotalRound()
{
	OnTotalRoundChange.Broadcast(TotalRound);
}

void AMutationController::ShowHUDSkill(bool bIsShow)
{
	OnSkillChange.Broadcast(bIsShow);
}

void AMutationController::SetHUDRage(float Rage)
{
	OnRageChange.Broadcast(Rage);
}

void AMutationController::SetHUDDamageMul(float DamageMul)
{
	OnDamageMulChange.Broadcast(DamageMul);
}
