#include "BaseMode.h"

#include "DataRegistryId.h"
#include "DataRegistrySubsystem.h"
#include "EngineUtils.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "BiochemicalArena/Equipments/Data/DamageTypeFall.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/UI/TextChat/TextChat.h"
#include "GameFramework/PlayerStart.h"

ABaseMode::ABaseMode()
{
	bDelayedStart = true;
}

void ABaseMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartTime = GetWorld()->GetTimeSeconds();
}

void ABaseMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABaseMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("OnPostLogin ------------------------------------------"));
	
	if (BaseGameState == nullptr) BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
	if (BaseGameState == nullptr) return;

	if (ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(NewPlayer->PlayerState))
	{
		BaseGameState->MulticastSendMsg(EMsgType::Join, BasePlayerState->GetTeam(), BasePlayerState->GetPlayerName());
	}
}

void ABaseMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	UE_LOG(LogTemp, Warning, TEXT("Logout ------------------------------------------"));
	
	if (BaseGameState == nullptr) BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
	if (BaseGameState == nullptr) return;

	if (ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(Exiting->PlayerState))
	{
		BaseGameState->MulticastSendMsg(EMsgType::Left, BasePlayerState->GetTeam(), BasePlayerState->GetPlayerName());
	}

	// 所有客户端退出后，再让服务端退出。
	if (MatchState == MatchState::LeavingMap && GetWorld()->GetNumPlayerControllers() == 2)
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			UE_LOG(LogTemp, Warning, TEXT("HandleLeavingMap ------------------------------------------"));
			PlayerController->ClientTravel("/Game/Maps/UI_Menu", ETravelType::TRAVEL_Absolute);
		}
	}
}

// 离开地图
void ABaseMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();

	// ABaseMode::Logout 中服务端退出失败，强制服务端退出。
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindWeakLambda(this, [this]() {
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			UE_LOG(LogTemp, Warning, TEXT("HandleLeavingMap ------------------------------------------"));
			PlayerController->ClientTravel("/Game/Maps/UI_Menu", ETravelType::TRAVEL_Absolute);
		}
	});
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.f, false);
}

// 生成人类角色
void ABaseMode::SpawnHumanCharacter(AController* Controller)
{
	if (Controller == nullptr) return;

	// 获取角色名字
	FString CharacterName;
	ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(Controller->PlayerState);
	if (BasePlayerState)
	{
		FString EnumValue = UEnum::GetValueAsString(BasePlayerState->GetHumanCharacterName());
		CharacterName = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
	}

	// 获取角色类
	TObjectPtr<UClass> CharacterClass;
	if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
	{
		FDataRegistryId DataRegistryId(DR_HUMAN_CHARACTER_MAIN, FName(CharacterName));
		if (const FHumanCharacterMain* HumanCharacterMain = DRSubsystem->GetCachedItem<FHumanCharacterMain>(DataRegistryId))
		{
			CharacterClass = HumanCharacterMain->HumanCharacterClass;
		}
	}
	if (CharacterClass == nullptr) return;

	// 获取出生点
	AActor* StartSpot = FindCharacterPlayerStart(BasePlayerState->GetTeam());
	if (StartSpot == nullptr) return;

	// 生成角色
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AHumanCharacter* HumanCharacter = GetWorld()->SpawnActor<AHumanCharacter>(
		CharacterClass,
		StartSpot->GetActorLocation(),
		StartSpot->GetActorRotation(),
		SpawnParams
	);

	// 搞搞Controller
	Controller->Possess(HumanCharacter);
	// 重置俯仰
	Controller->ClientSetRotation(HumanCharacter->GetActorRotation());
}

// 寻找角色出生点
AActor* ABaseMode::FindCharacterPlayerStart(ETeam Team)
{
	APlayerStart* PlayerStart = nullptr;

	if (Team == ETeam::Team1)
	{
		if (Team1PlayerStarts.IsEmpty())
		{
			for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
			{
				if (It->PlayerStartTag == "Team1")
				{
					Team1PlayerStarts.Add(*It);
				}
			}
		}

		PlayerStart = Team1PlayerStarts[FMath::RandRange(0, Team1PlayerStarts.Num() - 1)];
	}
	else if (Team == ETeam::Team2)
	{
		if (Team2PlayerStarts.IsEmpty())
		{
			for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
			{
				if (It->PlayerStartTag == "Team2")
				{
					Team2PlayerStarts.Add(*It);
				}
			}
		}

		PlayerStart = Team2PlayerStarts[FMath::RandRange(0, Team2PlayerStarts.Num() - 1)];
	}

	return PlayerStart;
}

// 分配队伍
void ABaseMode::AssignTeam(AController* Controller, ETeam Team)
{
	if (Controller == nullptr) return;

	if (BaseGameState == nullptr) BaseGameState = GetGameState<ABaseGameState>();
	if (BaseGameState == nullptr) return;

	if (ABasePlayerState* BasePlayerState = Controller->GetPlayerState<ABasePlayerState>())
	{
		BaseGameState->AddToTeam(BasePlayerState, Team);
		BasePlayerState->SetTeam(Team);

		BaseGameState->RemoveFromTeam(BasePlayerState, Team == ETeam::Team1 ? ETeam::Team2 : ETeam::Team1);
	}
}

// 击杀日志
void ABaseMode::AddKillLog(ABasePlayerState* AttackerState, AActor* DamageCauser, const UDamageType* DamageType, ABasePlayerState* DamagedState)
{
	FString CauserName = FString(TEXT("-1"));
	
	if (const UDamageTypeBase* DamageTypeBase = Cast<UDamageTypeBase>(DamageType))
	{
		switch (DamageTypeBase->DamageType)
		{
		case EDamageCauserType::Equipment:
			if (AEquipment* CauserEquipment = Cast<AEquipment>(DamageCauser->GetOwner()))
			{
				CauserName = UEnum::GetValueAsString(CauserEquipment->GetEquipmentName());
				CauserName = CauserName.Right(CauserName.Len() - CauserName.Find("::") - 2);
			}
			break;
		case EDamageCauserType::Melee:
			if (AEquipment* CauserEquipment = Cast<AEquipment>(DamageCauser))
			{
				CauserName = UEnum::GetValueAsString(CauserEquipment->GetEquipmentName());
				CauserName = CauserName.Right(CauserName.Len() - CauserName.Find("::") - 2);
			}
			break;
		case EDamageCauserType::MutantInfect:
			CauserName = FString(TEXT("Infect"));
			break;
		case EDamageCauserType::MutantDamage:
			CauserName = FString(TEXT("Damage"));
			break;
		case EDamageCauserType::Fall:
			CauserName = FString(TEXT("Fall"));
			break;
		}
	}

	if (BaseGameState == nullptr) BaseGameState = GetGameState<ABaseGameState>();
	if (BaseGameState)
	{
		BaseGameState->MulticastAddKillLog(AttackerState, CauserName, DamagedState);
	}
}
