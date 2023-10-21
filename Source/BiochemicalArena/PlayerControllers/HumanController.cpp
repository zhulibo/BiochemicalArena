#include "HumanController.h"
#include "BiochemicalArena/HUD/HumanHUD.h"
#include "BiochemicalArena/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"
#include "Net/UnrealNetwork.h"
#include "BiochemicalArena/HUD/Announcement.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "Kismet/GameplayStatics.h"

void AHumanController::BeginPlay()
{
	Super::BeginPlay();

	HumanHUD = Cast<AHumanHUD>(GetHUD());

	RequestServerMatchState();
	GetClientServerDelta();
}

void AHumanController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	PollInit();
}

void AHumanController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHumanController, MatchState);
}

void AHumanController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(InPawn);
	if (HumanCharacter)
	{
		SetHUDHealth(HumanCharacter->GetHealth(), HumanCharacter->GetMaxHealth());
	}
}

float AHumanController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AHumanController::GetClientServerDelta()
{
	if (IsLocalController())
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
	if (HumanHUD && MatchState == MatchState::WaitingToStart)
	{
		HumanHUD->AddAnnouncement();
	}
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
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD)
	{
		if (HumanHUD->CharacterOverlay == nullptr)
		{
			HumanHUD->AddCharacterOverlay();
		}
		if (HumanHUD->Announcement)
		{
			HumanHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AHumanController::HandleCooldown()
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD)
	{
		HumanHUD->CharacterOverlay->RemoveFromParent();
		if (HumanHUD->Announcement && HumanHUD->Announcement->AnnouncementText)
		{
			HumanHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			ATeamDeadMatchState* TeamDeadMatchState = Cast<ATeamDeadMatchState>(UGameplayStatics::GetGameState(this));
			AHumanState* HumanState = GetPlayerState<AHumanState>();
			if (TeamDeadMatchState && HumanState)
			{
				FString AnnouncementString = FString("Game over");
				TArray<AHumanState*> TopPlayers = TeamDeadMatchState->TopScoringPlayers;
				if (TopPlayers.Num() == 1)
				{
					AnnouncementString.Append(FString::Printf(TEXT(" Winner: \n%s"), *TopPlayers[0]->GetPlayerName()));
				}
				HumanHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementString));
			}
		}
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

void AHumanController::SetHUDWarmupCountdown(float CountdownTime)
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD && HumanHUD->Announcement && HumanHUD->Announcement->AnnouncementText)
	{
		if (CountdownTime < 0.f)
		{
			HumanHUD->Announcement->AnnouncementText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString AnnouncementString = FString::Printf(TEXT("Game starts in %02d:%02d"), Minutes, Seconds);
		HumanHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementString));
	}
}

void AHumanController::SetHUDMatchCountdown(float CountdownTime)
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	bool bHUDValid = HumanHUD && HumanHUD->CharacterOverlay && HumanHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			HumanHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HumanHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AHumanController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (HumanHUD && HumanHUD->CharacterOverlay)
		{
			CharacterOverlay = HumanHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void AHumanController::SetHUDHealth(float Health, float MaxHealth)
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD && HumanHUD->CharacterOverlay && HumanHUD->CharacterOverlay->HealthValue)
	{
		HumanHUD->CharacterOverlay->HealthValue->SetText(FText::AsNumber(Health));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AHumanController::SetHUDWeaponAmmo(int32 Ammo)
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD && HumanHUD->CharacterOverlay && HumanHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		HumanHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDCarriedAmmo(int32 Ammo)
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD && HumanHUD->CharacterOverlay && HumanHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		HumanHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDScore(float Score)
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD && HumanHUD->CharacterOverlay && HumanHUD->CharacterOverlay->ScoreAmount)
	{
		HumanHUD->CharacterOverlay->ScoreAmount->SetText(FText::AsNumber(Score));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void AHumanController::SetHUDDefeats(int32 Defeats)
{
	if (HumanHUD == nullptr)
	{
		HumanHUD = Cast<AHumanHUD>(GetHUD());
	}
	if (HumanHUD && HumanHUD->CharacterOverlay && HumanHUD->CharacterOverlay->DefeatsAmount)
	{
		HumanHUD->CharacterOverlay->DefeatsAmount->SetText(FText::AsNumber(Defeats));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}
