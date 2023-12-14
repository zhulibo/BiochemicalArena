#include "HumanController.h"
#include "CommonTextBlock.h"
#include "Components/TextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "Net/UnrealNetwork.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "BiochemicalArena/UI/HUD/CommonHUD.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/HUD/TeamDeadMatch.h"
#include "BiochemicalArena/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

void AHumanController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleClientServerDelta();
		RequestServerMatchState();

		HUDContainer->TeamDeadMatch->SetVisibility(ESlateVisibility::Visible);
	}
}

void AHumanController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		InitDefaultHUD();
		SetHUDTime();
	}
}

void AHumanController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHumanController, MatchState);
}

float AHumanController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AHumanController::HandleClientServerDelta()
{
	// 周期性获取ClientServerDelta
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	auto Lambda = [this]() {
		RequestServerTime(GetWorld()->GetTimeSeconds());
	};
	TimerDelegate.BindWeakLambda(this, Lambda);
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		TimerDelegate,
		RefreshFrequency,
		true,
		0.f
	);
}

void AHumanController::RequestServerTime_Implementation(float TimeClientRequest)
{
	float TimeServerReceived = GetWorld()->GetTimeSeconds();
	ReturnServerTime(TimeClientRequest, TimeServerReceived);
}

void AHumanController::ReturnServerTime_Implementation(float TimeClientRequest, float TimeServerReceived)
{
	float RoundTripNetworkDelay = GetWorld()->GetTimeSeconds() - TimeClientRequest; // 往返网络延迟
	ClientServerDelta = TimeServerReceived - TimeClientRequest - 0.5f * RoundTripNetworkDelay;
}

void AHumanController::RequestServerMatchState_Implementation()
{
	ATeamDeadMatchMode* GameMode = Cast<ATeamDeadMatchMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartTime = GameMode->LevelStartTime;
		MatchState = GameMode->GetMatchState();
		ReturnServerMatchState(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartTime);
	}
}

void AHumanController::ReturnServerMatchState_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
}

void AHumanController::OnMatchStateSet(FName State)
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

void AHumanController::OnRep_MatchState()
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

void AHumanController::HandleMatchHasStarted()
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->AnnouncementText->SetText(FText());
	}
}

void AHumanController::HandleCooldown()
{
	if (HUDContainer)
	{
		HUDContainer->CommonHUD->AnnouncementText->SetText(FText::FromString("Game over"));
	}
}

void AHumanController::InitDefaultHUD()
{
	if (bHasInitDefaultHUD) return;

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetPawn());
	if (HumanState == nullptr) HumanState = GetPlayerState<AHumanState>();

	if (HumanCharacter &&
		HumanCharacter->GetCombat() &&
		HumanCharacter->GetCombat()->GetCurrentWeapon() &&
		HumanState)
	{
		SetHUDAmmo(HumanCharacter->GetCombat()->GetCurrentWeapon()->GetAmmo());
		SetHUDCarriedAmmo(HumanCharacter->GetCombat()->GetCurrentWeapon()->GetCarriedAmmo());
		SetHUDHealth(HumanCharacter->GetHealth(), HumanCharacter->GetMaxHealth());
		SetHUDScore(HumanState->GetScore());
		SetHUDDefeat(HumanState->GetDefeat());

		bHasInitDefaultHUD = true;
	}
}

void AHumanController::SetHUDTime()
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

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
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

void AHumanController::SetHUDWarmupCountdown(uint32 CountdownTime)
{
	if (HUDContainer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString AnnouncementString = FString::Printf(TEXT("Game starts in %02d:%02d"), Minutes, Seconds);
		HUDContainer->CommonHUD->AnnouncementText->SetText(FText::FromString(AnnouncementString));
	}
}

void AHumanController::SetHUDMatchCountdown(uint32 CountdownTime)
{
	if (HUDContainer)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HUDContainer->TeamDeadMatch->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AHumanController::SetHUDHealth(float Health, float MaxHealth)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->HealthText->SetText(FText::AsNumber(Health));
	}
}

void AHumanController::SetHUDAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->WeaponAmmoText->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDCarriedAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->CarriedAmmoText->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDScore(float Score)
{
	// UE_LOG(LogTemp, Warning, TEXT("SetHUDScore Score: %d"), Score);
}

void AHumanController::SetHUDDefeat(int32 Defeat)
{
	// UE_LOG(LogTemp, Warning, TEXT("SetHUDDefeat Defeat: %d"), Defeat);
}
