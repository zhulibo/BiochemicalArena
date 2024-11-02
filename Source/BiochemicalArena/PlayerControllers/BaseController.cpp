#include "BaseController.h"

#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchGameState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/System/PlayerSubsystem.h"
#include "BiochemicalArena/UI/GameLayout.h"
#include "BiochemicalArena/UI/HUD/Mutation/HUDMutation.h"
#include "BiochemicalArena/UI/HUD/TeamDeadMatch/HUDTeamDeadMatch.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "GameFramework/PlayerState.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#define LOCTEXT_NAMESPACE "ABaseController"

void ABaseController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleServerClientDeltaTime();
	
		if (UPlayerSubsystem* PlayerSubsystem = ULocalPlayer::GetSubsystem<UPlayerSubsystem>(GetLocalPlayer()))
		{
			PlayerSubsystem->AddNotifyLayout();
		}
		
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

void ABaseController::SetPlayerSpectate()
{
	if (HasAuthority())
	{
		PlayerState->SetIsSpectator(true);
		ChangeState(NAME_Spectating);
		bPlayerIsWaiting = true;

		ClientGotoState(NAME_Spectating);
		ClientHUDStateChanged(EHUDState::Spectating);
	}
}

void ABaseController::SetPlayerPlay()
{
	if (HasAuthority())
	{
		PlayerState->SetIsSpectator(false);
		ChangeState(NAME_Playing);
		bPlayerIsWaiting = false;

		ClientGotoState(NAME_Playing);
		ClientHUDStateChanged(EHUDState::Playing);
	}
}

void ABaseController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	
	if(IsInState(NAME_Spectating))
	{
		ServerViewNextPlayer();
	}
}

void ABaseController::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	Super::SetViewTarget(NewViewTarget, TransitionParams);

	OnViewTargetChange.Broadcast(NewViewTarget);
}

void ABaseController::ClientHUDStateChanged_Implementation(EHUDState HUDState)
{
	OnHUDStateChange.Broadcast(HUDState);
}

void ABaseController::SetHUDWarmupCountdown(int32 CountdownTime)
{
	FString String = ULibraryCommon::GetFormatTime(CountdownTime);
	ChangeAnnouncement.Broadcast(FText::Format(LOCTEXT("GameStart", "Game starts in {0}"), FText::FromString(String)));
}

void ABaseController::HandleMatchHasStarted()
{
	ChangeAnnouncement.Broadcast(FText());
}

void ABaseController::HandleMatchHasEnded()
{
	ChangeAnnouncement.Broadcast(LOCTEXT("GameOver", "Game over"));
}

void ABaseController::HandleLeavingMap()
{
	if (!HasAuthority() && IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleLeavingMap ------------------------------------------"));
		ClientTravel("/Game/Maps/UI_Menu", ETravelType::TRAVEL_Absolute);
	}
}

void ABaseController::SetHUDAmmo(int32 Ammo)
{
	OnAmmoChange.Broadcast(Ammo);
}

void ABaseController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	OnCarriedAmmoChange.Broadcast(CarriedAmmo);
}

void ABaseController::ServerSendMsg_Implementation(EMsgType MsgType, ETeam Team, const FString& PlayerName, const FString& Msg)
{
	if (BaseGameState == nullptr) BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
	if (BaseGameState)
	{
		BaseGameState->MulticastSendMsg(MsgType, Team, PlayerName, Msg);
	}
}

#undef LOCTEXT_NAMESPACE
