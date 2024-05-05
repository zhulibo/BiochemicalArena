#include "BaseController.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/GameModes/BaseMode.h"
#include "BiochemicalArena/UI/HUD/CommonHUD.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/HUD/RadialMenuContainer.h"
#include "BiochemicalArena/UI/HUD/Scoreboard.h"
#include "BiochemicalArena/UI/HUD/TeamDeadMatch.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABaseController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
}

void ABaseController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleClientServerDelta();
		RequestServerMatchState();

		AddHUDContainer();
	}
}

void ABaseController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		SetHUDTime();
	}
}

void ABaseController::HandleClientServerDelta()
{
	// 周期性获取ClientServerDelta
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindWeakLambda(this, [this]() {
		RequestServerTime(GetWorld()->GetTimeSeconds());
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, RefreshFrequency, true, 0.f);
}

void ABaseController::RequestServerTime_Implementation(float TimeClientRequest)
{
	float TimeServerReceived = GetWorld()->GetTimeSeconds();
	ReturnServerTime(TimeClientRequest, TimeServerReceived);
}

void ABaseController::ReturnServerTime_Implementation(float TimeClientRequest, float TimeServerReceived)
{
	float RoundTripNetworkDelay = GetWorld()->GetTimeSeconds() - TimeClientRequest; // 往返网络延迟
	ClientServerDelta = TimeServerReceived - TimeClientRequest - 0.5f * RoundTripNetworkDelay;
}

float ABaseController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABaseController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABaseController::RequestServerMatchState_Implementation()
{
	if (BaseMode == nullptr) BaseMode = Cast<ABaseMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (BaseMode)
	{
		WarmupTime = BaseMode->GetWarmupTime();
		MatchTime = BaseMode->GetMatchTime();
		CooldownTime = BaseMode->GetCooldownTime();
		LevelStartTime = BaseMode->GetLevelStartTime();
		MatchState = BaseMode->GetMatchState();
		ReturnServerMatchState(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartTime);
	}
}

void ABaseController::ReturnServerMatchState_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
}

void ABaseController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

// TODO
void ABaseController::AddHUDContainer()
{
	if (!IsLocalController()) return;
	UObject* HUDContainerClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/HUD/W_HUDContainer.W_HUDContainer_C'"));
	if (HUDContainerClass) HUDContainer = CreateWidget<UHUDContainer>(this, Cast<UClass>(HUDContainerClass));
	if (HUDContainer)
	{
		HUDContainer->AddToViewport();
	}
}

void ABaseController::SetHUDTime()
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
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = LevelStartTime + CooldownTime + WarmupTime + MatchTime - GetServerTime();
	}

	int32 SecondsLeft = FMath::CeilToInt(TimeLeft);
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
		// 记录当前时间秒数
		CountdownSeconds = SecondsLeft;
	}
}

void ABaseController::SetHUDWarmupCountdown(int32 CountdownTime)
{
	if (HUDContainer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString AnnouncementString = FString::Printf(TEXT("Game starts in %02d:%02d"), Minutes, Seconds);
		HUDContainer->CommonHUD->Announcement->SetText(FText::FromString(AnnouncementString));
	}
}

void ABaseController::SetHUDMatchCountdown(int32 CountdownTime)
{
	if (HUDContainer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HUDContainer->TeamDeadMatch->MatchCountdown->SetText(FText::FromString(CountdownText));
	}
}

void ABaseController::HandleMatchHasStarted()
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->Announcement->SetText(FText());
	}
}

void ABaseController::HandleCooldown()
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->Announcement->SetText(FText::FromString("Game over"));
	}
}

void ABaseController::ShowScoreboard(bool bIsShow)
{
	if (HUDContainer && HUDContainer->Scoreboard)
	{
		if (bIsShow)
		{
			HUDContainer->Scoreboard->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			HUDContainer->Scoreboard->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABaseController::ShowPauseMenu()
{
	if (HUDContainer)
	{
		HUDContainer->ShowPauseMenu();
	}
}

void ABaseController::AddKillLog(ABasePlayerState* AttackerState, const FString& EquipmentName, ABasePlayerState* KilledState)
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->AddKillLog(AttackerState, EquipmentName, KilledState);
	}
}

void ABaseController::ShowRadialMenu()
{
	if (HUDContainer)
	{
		HUDContainer->RadialMenuContainer->ShowRadialMenu();
	}
}

void ABaseController::ChangeRadialMenu()
{
	if (HUDContainer)
	{
		HUDContainer->RadialMenuContainer->ChangeRadialMenu();
	}
}

void ABaseController::SelectRadialMenu(double X, double Y)
{
	if (HUDContainer)
	{
		HUDContainer->RadialMenuContainer->SelectRadialMenu(X, Y);
	}
}

void ABaseController::CloseRadialMenu()
{
	if (HUDContainer)
	{
		HUDContainer->RadialMenuContainer->CloseRadialMenu();
	}
}
