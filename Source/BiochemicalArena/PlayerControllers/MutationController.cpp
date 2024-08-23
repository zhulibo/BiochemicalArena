#include "MutationController.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/PlayerStates/MutationPlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/UI/HUD/CommonHUD.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/HUD/Mutation.h"
#include "BiochemicalArena/UI/HUD/MutationHuman.h"
#include "BiochemicalArena/UI/HUD/MutationMutant.h"
#include "BiochemicalArena/UI/HUD/RadialMenu/RadialMenuContainer.h"
#include "Components/HorizontalBox.h"
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

		if (HUDContainer)
		{
			HUDContainer->Mutation->SetVisibility(ESlateVisibility::Visible);
			HUDContainer->MutationHuman->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AMutationController::OnTeamChange(ETeam TempTeam)
{
	if (HUDContainer)
	{
		if (TempTeam == ETeam::Team1)
		{
			HUDContainer->MutationHuman->SetVisibility(ESlateVisibility::Visible);
			HUDContainer->MutationMutant->SetVisibility(ESlateVisibility::Collapsed);

			HUDContainer->RadialMenuContainer->OnTeamChange(ETeam::Team1);
		}
		else if (TempTeam == ETeam::Team2)
		{
			HUDContainer->MutationHuman->SetVisibility(ESlateVisibility::Collapsed);
			HUDContainer->MutationMutant->SetVisibility(ESlateVisibility::Visible);

			HUDContainer->RadialMenuContainer->OnTeamChange(ETeam::Team2);

			bCanSelectCharacter = true;
			ShowSelectCharacterTip(true);
			GetWorld()->GetTimerManager().SetTimer(DisableSelectTimerHandle, this, &ThisClass::DisableSelectCharacter, 10.f);
		}
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
	if (IsLocalController())
	{
		GetWorld()->GetTimerManager().ClearTimer(DisableSelectTimerHandle);
		DisableSelectCharacter();
	}

	if (HUDContainer)
	{
		HUDContainer->CommonHUD->Announcement->SetText(FText::FromString("Round end"));
	}
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
			SetHUDMatchCountdown(SecondsLeft);

			// 突变倒计时
			int32 MutateCountdown = FMath::RoundToInt(MutateTime + TimeLeft - RoundTime);
			if (MutateCountdown > 0)
			{
				if (HUDContainer)
				{
					HUDContainer->CommonHUD->Announcement->SetText(FText::FromString(FString::Printf(TEXT("%d"), MutateCountdown)));
				}
				// TODO 声音
			}
			else if (MutateCountdown == 0)
			{
				if (HUDContainer)
				{
					HUDContainer->CommonHUD->Announcement->SetText(FText());
				}
			}
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

void AMutationController::SetHUDMatchCountdown(int32 CountdownTime)
{
	if (HUDContainer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HUDContainer->Mutation->MatchCountdown->SetText(FText::FromString(CountdownText));
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

		bNeedInitHUD = false;
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

		UpdateRageUI(MutationPlayerState->GetRage());

		if (MutationPlayerState->GetAbilitySystemComponent())
		{
			FGameplayTag SkillCooldownTag = FGameplayTag::RequestGameplayTag(TAG_SKILL_CD);
			int32 TagCount = MutationPlayerState->GetAbilitySystemComponent()->GetTagCount(SkillCooldownTag);
			ShowSkillUI(TagCount == 0.f && MutationPlayerState->GetCharacterLevel() >= 2.f);
		}

		bNeedInitHUD = false;
	}
}

void AMutationController::SetHUDHealth(float Health)
{
	if (MutationPlayerState == nullptr) MutationPlayerState = Cast<AMutationPlayerState>(PlayerState);
	if (HUDContainer && MutationPlayerState)
	{
		FNumberFormattingOptions Opts;
		Opts.RoundingMode = ERoundingMode::ToPositiveInfinity; // 向上取整
		Opts.SetUseGrouping(false); // 不使用千位分隔符

		if (MutationPlayerState->GetTeam() == ETeam::Team1)
		{
			HUDContainer->MutationHuman->Health->SetText(FText::AsNumber(Health, &Opts));
		}
		else if(MutationPlayerState->GetTeam() == ETeam::Team2)
		{
			HUDContainer->MutationMutant->Health->SetText(FText::AsNumber(Health, &Opts));
		}
	}
}

void AMutationController::SetHUDAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->MutationHuman->Ammo->SetText(FText::AsNumber(Ammo));
	}
}

void AMutationController::SetHUDCarriedAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->MutationHuman->CarriedAmmo->SetText(FText::AsNumber(Ammo));
	}
}

void AMutationController::SetHUDTeamNum(int32 TeamNum, ETeam Team)
{
	if (HUDContainer)
	{
		switch (Team)
		{
		case ETeam::Team1:
			HUDContainer->Mutation->HumanNumber->SetText(FText::AsNumber(TeamNum));
			break;
		case ETeam::Team2:
			HUDContainer->Mutation->MutantNumber->SetText(FText::AsNumber(TeamNum));
			break;
		}
	}
}

void AMutationController::SetHUDCurrentRound()
{
	if (HUDContainer)
	{
		HUDContainer->Mutation->CurrentRound->SetText(FText::AsNumber(CurrentRound));
	}
}

void AMutationController::SetHUDTotalRound()
{
	if (HUDContainer)
	{
		HUDContainer->Mutation->TotalRound->SetText(FText::AsNumber(TotalRound));
	}
}

void AMutationController::Show1000DamageUI()
{
	if (HUDContainer)
	{
		HUDContainer->MutationHuman->DamageIcon->SetText(FText::FromString("1000 DAMAGE"));

		GetWorldTimerManager().SetTimer(DamageIconTimerHandle, this, &ThisClass::Clear1000DamageUI, 5.f);
	}
}

void AMutationController::Clear1000DamageUI()
{
	if (HUDContainer)
	{
		HUDContainer->MutationHuman->DamageIcon->SetText(FText::FromString(""));
	}
}

void AMutationController::ShowCharacterMenu()
{
	if (HUDContainer && bCanSelectCharacter)
	{
		HUDContainer->ActivateWidget();
		FInputModeUIOnly InputModeData;
		SetInputMode(InputModeData);
		SetShowMouseCursor(true);

		HUDContainer->ShowCharacterMenu();
	}
}

void AMutationController::DisableSelectCharacter()
{
	bCanSelectCharacter = false;
	ShowSelectCharacterTip(false);
}

void AMutationController::ShowSelectCharacterTip(bool bIsShow)
{
	if (HUDContainer)
	{
		HUDContainer->MutationMutant->ShowSelectCharacterTip(bIsShow);
	}
}

void AMutationController::ShowSkillUI(bool bIsShow)
{
	if (HUDContainer)
	{
		HUDContainer->MutationMutant->ShowSkillUI(bIsShow);
	}
}

void AMutationController::UpdateRageUI(float Rage)
{
	if (HUDContainer)
	{
		HUDContainer->MutationMutant->UpdateRageUI(Rage);
	}
}

void AMutationController::SetHUDDamageMul(float DamageMul)
{
	if (HUDContainer)
	{
		HUDContainer->MutationHuman->DamageMul->SetText(FText::Format(FText::FromString("{0}%"), FMath::FloorToInt(DamageMul * 100)));
	}
}
