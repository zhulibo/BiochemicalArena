#include "BaseController.h"
#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchGameState.h"
#include "BiochemicalArena/UI/GameLayout.h"
#include "BiochemicalArena/UI/HUD/Mutation/HUDMutation.h"
#include "BiochemicalArena/UI/HUD/TeamDeadMatch/HUDTeamDeadMatch.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void ABaseController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleServerClientDeltaTime();
		
		AddGameLayout();
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

void ABaseController::ManualReset()
{
	InitHUD();
	
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

void ABaseController::AddGameLayout()
{
	GameLayout = CreateWidget<UGameLayout>(this, GameLayoutClass);
	if (GameLayout)
	{
		GameLayout->AddToViewport();
		
		FocusGame();

		if (Cast<AMutationGameState>(GetWorld()->GetGameState()))
		{
			GameLayout->GameStack->AddWidget(GameLayout->HUDMutationClass);
		}
		else if (Cast<ATeamDeadMatchGameState>(GetWorld()->GetGameState()))
		{
			GameLayout->GameStack->AddWidget(GameLayout->HUDTeamDeadMatchClass);
		}
	}
}

void ABaseController::FocusGame()
{
	FInputModeGameOnly InputModeData;
	SetInputMode(InputModeData);
	SetShowMouseCursor(false);
}

void ABaseController::FocusUI()
{
	FInputModeUIOnly InputModeData;
	SetInputMode(InputModeData);
	SetShowMouseCursor(true);
}

void ABaseController::SetHUDWarmupCountdown(int32 CountdownTime)
{
	FString String = ULibraryCommon::GetFormatTime(CountdownTime);
	String = FString::Printf(TEXT("Game starts in %s"), *String);
	ChangeAnnouncement.Broadcast(FText::FromString(String));
}

void ABaseController::HandleMatchHasStarted()
{
	ChangeAnnouncement.Broadcast(FText());
}

void ABaseController::HandleMatchHasEnded()
{
	ChangeAnnouncement.Broadcast(FText::FromString("Game over"));
}

void ABaseController::SetHUDAmmo(int32 Ammo)
{
	OnAmmoChange.Broadcast(Ammo);
}

void ABaseController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	OnCarriedAmmoChange.Broadcast(CarriedAmmo);
}
