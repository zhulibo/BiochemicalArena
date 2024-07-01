#include "BaseMode.h"

#include "EngineUtils.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "BiochemicalArena/Equipments/DamageTypes/FallDamageType.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
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

// 生成人类角色
void ABaseMode::SpawnHumanCharacter(AController* Controller)
{
	if (Controller == nullptr) return;

	// 获取角色名字
	FString CharacterName;
	ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(Controller->PlayerState);
	if (BasePlayerState)
	{
		CharacterName = BasePlayerState->GetHumanCharacterName();
	}

	// 获取角色类
	FString CharacterClassPath = FString::Printf(TEXT("/Script/Engine.Blueprint'/Game/Characters/Humans/%s/BP_%s.BP_%s_C'"), *CharacterName, *CharacterName, *CharacterName);
	UClass* CharacterClass = StaticLoadClass(UObject::StaticClass(), nullptr, *CharacterClassPath);
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
	Controller->ClientSetRotation(HumanCharacter->GetActorRotation());
	// TODO On server side, ROLE_SimulatedProxy's rotation has a delay.
}

// 生成突变体角色
void ABaseMode::SpawnMutantCharacter(AController* Controller, FVector Location, FRotator ActorRotation, FRotator ViewRotation)
{
	if (Controller == nullptr) return;

	// 获取角色名字
	FString CharacterName;
	ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(Controller->PlayerState);
	if (BasePlayerState)
	{
		CharacterName = BasePlayerState->GetMutantCharacterName();
	}

	// 获取角色类
	FString CharacterClassPath = FString::Printf(TEXT("/Script/Engine.Blueprint'/Game/Characters/Mutants/%s/BP_%s.BP_%s_C'"), *CharacterName, *CharacterName, *CharacterName);
	UClass* CharacterClass = StaticLoadClass(UObject::StaticClass(), nullptr, *CharacterClassPath);
	if (CharacterClass == nullptr) return;

	// 未传入位置和旋转，使用出生点的位置和旋转
	if (Location == FVector::ZeroVector || ActorRotation == FRotator::ZeroRotator)
	{
		AActor* StartSpot = FindCharacterPlayerStart(ETeam::Team2);
		if (StartSpot)
		{
			Location = StartSpot->GetActorLocation();
			ActorRotation = StartSpot->GetActorRotation();
		}
	}

	// 生成角色
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AMutantCharacter* MutantCharacter = GetWorld()->SpawnActor<AMutantCharacter>(
		CharacterClass,
		Location,
		ActorRotation,
		SpawnParams
	);

	Controller->Possess(MutantCharacter);

	// 设置俯仰
	if (ViewRotation == FRotator::ZeroRotator)
	{
		Controller->ClientSetRotation(MutantCharacter->GetActorRotation());
	}
	else
	{
		Controller->ClientSetRotation(ViewRotation);
	}
	// TODO On server side, ROLE_SimulatedProxy's rotation has a delay.
}

// 寻找角色出生点
AActor* ABaseMode::FindCharacterPlayerStart(ETeam Team)
{
	APlayerStart* PlayerStart = nullptr;

	if (Team == ETeam::Team1)
	{
		if (Team1PlayerStarts.Num() == 0)
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
		if (Team2PlayerStarts.Num() == 0)
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
	ABasePlayerState* BasePlayerState = Controller->GetPlayerState<ABasePlayerState>();

	if (BaseGameState && BasePlayerState)
	{
		BaseGameState->AddToTeam(BasePlayerState, Team);
		BasePlayerState->SetTeam(Team);

		BaseGameState->RemoveFromTeam(BasePlayerState, Team == ETeam::Team1 ? ETeam::Team2 : ETeam::Team1);
	}
}

// 击杀日志
void ABaseMode::AddKillLog(ABasePlayerState* AttackerState, AActor* DamageCauser, const UDamageType* DamageType, ABasePlayerState* DamagedState)
{
	FString CauserName = "-1";
	const UBaseDamageType* BaseDamageType = Cast<UBaseDamageType>(DamageType);

	// Equipment击杀
	if (BaseDamageType->DamageType == EDamageType2::Equipment)
	{
		// DamageCauser is Projectile, Projectile's owner is Equipment
		if (AEquipment* CauserEquipment1 = Cast<AEquipment>(DamageCauser->GetOwner()))
		{
			CauserName = UEnum::GetValueAsString(CauserEquipment1->GetEquipmentName());
			CauserName = CauserName.Right(CauserName.Len() - CauserName.Find("::") - 2);
		}
		// DamageCauser is Melee
		else if (AEquipment* CauserEquipment2 = Cast<AEquipment>(DamageCauser))
		{
			CauserName = UEnum::GetValueAsString(CauserEquipment2->GetEquipmentName());
			CauserName = CauserName.Right(CauserName.Len() - CauserName.Find("::") - 2);
		}
	}
	// 感染 摔死 掉出地图
	else
	{
		CauserName = BaseDamageType->CauserName;
	}

	if (BaseGameState == nullptr) BaseGameState = GetGameState<ABaseGameState>();
	if (BaseGameState)
	{
		BaseGameState->MulticastAddKillLog(AttackerState, CauserName, DamagedState);
	}
}
