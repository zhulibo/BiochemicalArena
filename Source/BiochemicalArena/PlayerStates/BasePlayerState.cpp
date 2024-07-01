#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TeamType.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/Characters/Components/OverheadWidget.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

ABasePlayerState::ABasePlayerState()
{
	Team = ETeam::NoTeam;
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, Damage);
	DOREPLIFETIME(ThisClass, Defeat);
	DOREPLIFETIME(ThisClass, KillStreak);
}

void ABasePlayerState::BeginPlay()
{
	Super::BeginPlay();

	BaseController = Cast<ABaseController>(GetOwner());
	if (BaseController && BaseController->IsLocalController())
	{
		UStorageSubsystem* StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
		if (StorageSubsystem && StorageSubsystem->StorageCache)
		{
			ServerSetHumanCharacterName(StorageSubsystem->StorageCache->Character);
		}
	}
}

void ABasePlayerState::SetTeam(ETeam TempTeam)
{
	// UE_LOG(LogTemp, Warning, TEXT("SetTeam -----------------------"));

	Team = TempTeam;
	BaseCharacter = nullptr; // TODO 被销毁时自动置为nullptr
}

void ABasePlayerState::OnRep_Team()
{
	// UE_LOG(LogTemp, Warning, TEXT("OnRep_Team ----------------------"));

	BaseCharacter = nullptr; // TODO 被销毁时自动置为nullptr

	BaseCharacter = Cast<ABaseCharacter>(GetPawn());
	if (BaseCharacter)
	{
		BaseCharacter->HasInitMeshCollision = false;
	}

	SetPlayerNameTeamColor();
}

// 设置PlayerName队伍颜色
void ABasePlayerState::SetPlayerNameTeamColor()
{
	if (BaseCharacter == nullptr) BaseCharacter = Cast<ABaseCharacter>(GetPawn());
	if (BaseCharacter)
	{
		if (!BaseCharacter->IsLocallyControlled())
		{
			if (UWidgetComponent* OverheadWidget = BaseCharacter->GetOverheadWidget())
			{
				if (UOverheadWidget* OverheadWidgetClass = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject()))
				{
					OverheadWidgetClass->SetPlayerNameTeamColor();
					return;
				}
			}
		}
		// 本地玩家队伍改变时，更新所有玩家的队伍颜色
		else
		{
			// double Time1 = FPlatformTime::Seconds();

			TArray<AActor*> AllPlayers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), AllPlayers);
			for (AActor* Player : AllPlayers)
			{
				if (ABaseCharacter* IgnoreBaseCharacter = Cast<ABaseCharacter>(Player))
				{
					if (UWidgetComponent* OverheadWidget = IgnoreBaseCharacter->GetOverheadWidget())
					{
						if (UOverheadWidget* OverheadWidgetClass = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject()))
						{
							OverheadWidgetClass->SetPlayerNameTeamColor();
						}
					}
				}
			}

			// double Time2 = FPlatformTime::Seconds();
			// UE_LOG(LogTemp, Warning, TEXT("SetPlayerNameTeamColor %f"), Time2 - Time1);

			return;
		}
	}

	GetWorldTimerManager().SetTimerForNextTick([this]() {
		SetPlayerNameTeamColor();
	});
}

void ABasePlayerState::ServerSetHumanCharacterName_Implementation(const FString& TempHumanCharacterName)
{
	HumanCharacterName = TempHumanCharacterName;
}

void ABasePlayerState::SetMutantCharacterName(const FString& TempMutantCharacterName)
{
	MutantCharacterName = TempMutantCharacterName;
}

void ABasePlayerState::AddDamage(float TempDamage)
{
	Damage += TempDamage;
}

void ABasePlayerState::OnRep_Damage()
{
}

void ABasePlayerState::AddDefeat()
{
	Defeat++;
}

void ABasePlayerState::OnRep_Defeat()
{
}

void ABasePlayerState::AddKillStreak()
{
	KillStreak++;
	GetWorldTimerManager().SetTimer(ResetKillStreakTimerHandle, this, &ThisClass::ResetKillStreak, 5.f);

	ShowKillStreak();
}

void ABasePlayerState::ResetKillStreak()
{
	KillStreak = 0;
}

void ABasePlayerState::OnRep_KillStreak()
{
	ShowKillStreak();
}

void ABasePlayerState::ShowKillStreak()
{
	if (KillStreak > 1)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwner());
		if (BaseController && BaseController->IsLocalController())
		{
			BaseController->ShowKillStreak(KillStreak);
			GetWorldTimerManager().SetTimer(ClearKillStreakTimerHandle, this, &ThisClass::HiddenKillStreak, 10.f);
		}
	}
}

void ABasePlayerState::HiddenKillStreak()
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwner());
	if (BaseController && BaseController->IsLocalController())
	{
		BaseController->HiddenKillStreak();
	}
}
