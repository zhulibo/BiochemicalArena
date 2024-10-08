#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TeamType.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/Abilities/BAAbilitySystemComponent.h"
#include "BiochemicalArena/Abilities/AttributeSetBase.h"
#include "BiochemicalArena/Characters/Components/OverheadWidget.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/Storage/DefaultConfig.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UBAAbilitySystemComponent>(TEXT("BaseAbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSetBase = CreateDefaultSubobject<UAttributeSetBase>(TEXT("BaseAttributeSet"));

	if (const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>())
	{
		HumanCharacterName = DefaultConfig->HumanCharacterName;
		MutantCharacterName = DefaultConfig->MutantCharacterName;
	}

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
		if (StorageSubsystem && StorageSubsystem->CacheSetting)
		{
			ServerSetHumanCharacterName(StorageSubsystem->CacheSetting->HumanCharacterName);
			ServerSetMutantCharacterName(StorageSubsystem->CacheSetting->MutantCharacterName);
		}
	}

	InitData();
}

void ABasePlayerState::Reset()
{
	Super::Reset();

	InitData();
}

// 初始化数据
void ABasePlayerState::InitData()
{
	if (AttributeSetBase && GetCharacterLevel() != 2.f)
	{
		AttributeSetBase->SetCharacterLevel(2.f);
	}
}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetBase* ABasePlayerState::GetAttributeSetBase()
{
	return AttributeSetBase;
}

float ABasePlayerState::GetMaxHealth()
{
	return AttributeSetBase ? AttributeSetBase->GetMaxHealth() : 0.f;
}

float ABasePlayerState::GetHealth()
{
	return AttributeSetBase ? AttributeSetBase->GetHealth() : 0.f;
}

float ABasePlayerState::GetDamageReceivedMul()
{
	return AttributeSetBase ? AttributeSetBase->GetDamageReceivedMul() : 0.f;
}

float ABasePlayerState::GetRepelReceivedMul()
{
	return AttributeSetBase ? AttributeSetBase->GetRepelReceivedMul() : 0.f;
}

float ABasePlayerState::GetCharacterLevel()
{
	return AttributeSetBase ? AttributeSetBase->GetCharacterLevel() : 0.f;
}

float ABasePlayerState::GetJumpZVelocity()
{
	return AttributeSetBase ? AttributeSetBase->GetJumpZVelocity() : 0.f;
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

	InitOverheadWidget();
}

void ABasePlayerState::InitOverheadWidget()
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
					OverheadWidgetClass->InitOverheadWidget();
					return;
				}
			}
		}
		// 本地玩家队伍改变时，初始化本机所有玩家的OverheadWidget
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
							OverheadWidgetClass->InitOverheadWidget();
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
		InitOverheadWidget();
	});
}

void ABasePlayerState::ServerSetHumanCharacterName_Implementation(EHumanCharacterName TempHumanCharacterName)
{
	HumanCharacterName = TempHumanCharacterName;
}

void ABasePlayerState::ServerSetMutantCharacterName_Implementation(EMutantCharacterName TempMutantCharacterName)
{
	MutantCharacterName = TempMutantCharacterName;
}

void ABasePlayerState::SetMutantCharacterName(EMutantCharacterName TempMutantCharacterName)
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
