#include "BaseController.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/HUD/CommonHUD.h"
#include "BiochemicalArena/UI/HUD/Crosshair.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/HUD/RadialMenuContainer.h"
#include "BiochemicalArena/UI/HUD/Scoreboard.h"

void ABaseController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleServerClientDeltaTime();

		AddHUDContainer();
	}
}

void ABaseController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		if (bNeedInitHUD)
		{
			InitHUD();
		}

		SetHUDTime();
	}
}

void ABaseController::ManualReset()
{
	bNeedInitHUD = true;

	BaseCharacter = nullptr; // TODO 被销毁时自动置为nullptr
}

void ABaseController::HandleServerClientDeltaTime()
{
	// 周期性获取ServerClientDeltaTime
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindWeakLambda(this, [this]() {
		RequestServerTime(GetWorld()->GetTimeSeconds());
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.f, true, 0.f);
}

void ABaseController::RequestServerTime_Implementation(float ClientTime)
{
	ReturnServerTime(ClientTime, GetWorld()->GetTimeSeconds());
}

void ABaseController::ReturnServerTime_Implementation(float ClientTime, float ServerTime)
{
	float RoundTripNetworkDelay = GetWorld()->GetTimeSeconds() - ClientTime; // 往返网络延迟
	ServerClientDeltaTime = ServerTime - ClientTime - RoundTripNetworkDelay * 0.5f;
}

float ABaseController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	return GetWorld()->GetTimeSeconds() + ServerClientDeltaTime;
}

void ABaseController::AddHUDContainer()
{
	HUDContainer = CreateWidget<UHUDContainer>(this, HUDContainerClass);
	if (HUDContainer)
	{
		HUDContainer->AddToViewport();
		FInputModeGameOnly InputModeData;
		SetInputMode(InputModeData);
		SetShowMouseCursor(false);
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

void ABaseController::HandleMatchHasStarted()
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->Announcement->SetText(FText());
	}
}

void ABaseController::HandleMatchHasEnded()
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->Announcement->SetText(FText::FromString("Game over"));
	}
}

void ABaseController::InitHUD()
{
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

void ABaseController::AddKillLog(ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState)
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->AddKillLog(AttackerState, CauserName, DamagedState);
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

void ABaseController::SetHUDCrosshair(float CrosshairSpread)
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->Crosshair->SetCrosshairSpread(CrosshairSpread);
	}
}

void ABaseController::ShowKillStreak(int32 KillStreak)
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->KillStreak->SetText(FText::FromString(FString::Printf(TEXT("%d KILL"), KillStreak)));
	}
}

void ABaseController::HiddenKillStreak()
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->KillStreak->SetText(FText::FromString(""));
	}
}
