#include "MutationMode.h"

#include "DataRegistrySubsystem.h"
#include "EngineUtils.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/MutationPlayerState.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "BiochemicalArena/Equipments/Data/DamageTypeMutantInfect.h"
#include "BiochemicalArena/Equipments/Data/DamageTypeMutantDamage.h"
#include "BiochemicalArena/Equipments/Pickups/Pickup.h"
#include "BiochemicalArena/Equipments/Projectiles/ProjectileBullet.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/System/DevSetting.h"
#include "GameFramework/PlayerStart.h"

namespace MatchState
{
	const FName PostRound = FName("PostRound");
}

void AMutationMode::BeginPlay()
{
	Super::BeginPlay();

	MutationGameState = GetGameState<AMutationGameState>();
}

void AMutationMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 比赛前
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = LevelStartTime + WarmupTime - GetWorld()->GetTimeSeconds();
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	// 对局中（一场比赛有多局）
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = RoundStartTime + RoundTime - GetWorld()->GetTimeSeconds();
		if (CountdownTime <= 0.f)
		{
			EndRound();
		}
	}
	// 对局后
	else if (MatchState == MatchState::PostRound)
	{
		CountdownTime = RoundEndTime + PostRoundTime - GetWorld()->GetTimeSeconds();
		if (CountdownTime <= 0.f)
		{
			if (CurrentRound < TotalRound)
			{
				StartNextRound();
			}
			else
			{
				EndMatch();
			}
		}
	}
	// 比赛后
	else if (MatchState == MatchState::WaitingPostMatch)
	{
		CountdownTime = RoundEndTime + PostRoundTime + CooldownTime - GetWorld()->GetTimeSeconds();
		if (CountdownTime <= 0.f)
		{
			StartToLeaveMap();
		}
	}
}

bool AMutationMode::IsMatchInProgress() const
{
	if (GetMatchState() == MatchState::PostRound)
	{
		return true;
	}

	return Super::IsMatchInProgress();
}

// 开始游戏（开始第一回合）
void AMutationMode::StartMatch()
{
	RoundStartTime = GetWorld()->GetTimeSeconds();

	Super::StartMatch();
}

// 结束回合
void AMutationMode::EndRound()
{
	// 对局时间结束时结束监视对局状态
	bWatchRoundState = false;

	// 清除生成补给箱定时器
	GetWorldTimerManager().ClearTimer(SpawnPickupTimerHandle);

	RoundEndTime = GetWorld()->GetTimeSeconds();
	SetMatchState(MatchState::PostRound);
}

// 开启下一回合
void AMutationMode::StartNextRound()
{
	CurrentRound++;
	RoundStartTime = GetWorld()->GetTimeSeconds();
	SetMatchState(MatchState::InProgress);
}

// 结束比赛
void AMutationMode::EndMatch()
{
	if (bIsEndingMatch) return;
	bIsEndingMatch = true;

	// 比赛时间结束时结束监视比赛状态
	bWatchMatchState = false;

	// 清除生成补给箱定时器
	GetWorldTimerManager().ClearTimer(SpawnPickupTimerHandle);

	Super::EndMatch();
}

// MatchState变化时通知所有Controller
void AMutationMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AMutationController* MutationController = Cast<AMutationController>(*It))
		{
			MutationController->OnMatchStateSet(MatchState, CurrentRound);
		}
	}
}

// 对局开始
void AMutationMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// 重新开始回合时，重置状态
	if (CurrentRound > 1)
	{
		// 重置关卡
		ResetLevel();

		// 销毁所有装备
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			if (AEquipment* Equipment = Cast<AEquipment>(*It))
			{
				Equipment->Destroy();
			}
		}
	}

	// 生成角色
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AMutationController* MutationController = Cast<AMutationController>(*It))
		{
			MutationController->SetPlayerPlay();

			AssignTeam(MutationController, ETeam::Team1);

			SpawnHumanCharacter(MutationController);
		}
	}

	// 第一局角色生成后开始监视比赛状态
	if (CurrentRound == 1)
	{
		bWatchMatchState = true;
	}

	// 倒计时结束，选择人类突变
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::RoundStartMutate, MutateTime);
}

// 中途加入
void AMutationMode::OnPostLogin(AController* Controller)
{
	Super::OnPostLogin(Controller);

	// 生成角色
	if (MatchState == MatchState::InProgress)
	{
		AssignTeam(Controller, ETeam::Team1);

		SpawnHumanCharacter(Controller);
	}
}

// 开局随机选择人类突变
void AMutationMode::RoundStartMutate()
{
	if (MutationGameState == nullptr) MutationGameState = GetGameState<AMutationGameState>();
	if (MutationGameState == nullptr) return;

	TArray<ABasePlayerState*> Team1 = MutationGameState->GetTeam(ETeam::Team1);

	int32 MutateHumanNum = 1;
	if (Team1.Num() >= 8 && Team1.Num() <= 14)
	{
		MutateHumanNum = 2;
	}
	else if (Team1.Num() >= 15 && Team1.Num() <= 18)
	{
		MutateHumanNum = 3;
	}

	for (int i = 0; i < MutateHumanNum; ++i)
	{
		int32 RandomIndex = FMath::RandRange(0, Team1.Num() - 1);
		
#if UE_EDITOR
		RandomIndex = GetDefault<UDevSetting>()->MutateClientIndex;

		if (RandomIndex > Team1.Num() - 1)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.f, C_RED, TEXT("MutateClientIndex too Big!"), false);
			RandomIndex = Team1.Num() - 1;
		}
#endif

		if (ABasePlayerState* BasePlayerState = Team1[RandomIndex])
		{
			if (AController* Controller = Cast<AController>(BasePlayerState->GetOwner()))
			{
				Mutate(Controller->GetCharacter(), Controller, ESpawnReason::RoundStart);
			}
		}
	}
	// 突变后开始监视对局状态
	bWatchRoundState = true;

	// 定时生成补给箱
	// GetWorldTimerManager().SetTimer(SpawnPickupTimerHandle, this, &ThisClass::SpawnPickups, 5.f, false);
	GetWorldTimerManager().SetTimer(SpawnPickupTimerHandle, this, &ThisClass::SpawnPickups, 40.f, true);
}

// 人类受到伤害
void AMutationMode::HumanReceiveDamage(AHumanCharacter* DamagedCharacter, ABaseController* DamagedController,
	float Damage, const UDamageType* DamageType, AController* AttackerController, AActor* DamageCauser)
{
	if (DamagedCharacter == nullptr || DamagedController == nullptr || AttackerController == nullptr || DamageCauser == nullptr) return;

	AMutationPlayerState* DamagedState = Cast<AMutationPlayerState>(DamagedController->PlayerState);
	AMutationPlayerState* AttackerState = Cast<AMutationPlayerState>(AttackerController->PlayerState);

	if (DamagedState == nullptr || AttackerState == nullptr) return;

	// 设置受伤者血量
	float TakenDamage = FMath::Clamp(Damage, 0.f, DamagedCharacter->GetHealth());
	float Health = DamagedCharacter->GetHealth() - TakenDamage;
	DamagedCharacter->SetHealth(Health);
	DamagedCharacter->MulticastSetHealth(Health, AttackerController);

	if (AttackerState != DamagedState) // 受到跌落伤害时，AttackerController和DamageCauser传的是自己
	{
		// 增加攻击者怒气值
		AttackerState->SetRage(AttackerState->GetRage() + Damage * 10);
	}

	// 人类死亡
	if (DamagedCharacter->GetHealth() <= 0.f)
	{
		if (AttackerState != DamagedState)
		{
			// 增加攻击者连杀
			AttackerState->AddKillStreak();
		}

		// 击杀日志
		AddKillLog(AttackerState, DamageCauser, DamageType, DamagedState);

		// 人类死亡后突变
		if (const UDamageTypeBase* DamageTypeBase = Cast<UDamageTypeBase>(DamageType))
		{
			if (DamageTypeBase->DamageType != EDamageCauserType::MutantDamage)
			{
				DamagedCharacter->MulticastMutationDead(true);
				return;
			}
		}
		Mutate(DamagedCharacter, DamagedController, ESpawnReason::MutantDamage);
	}
}

// 人类受到感染
void AMutationMode::GetInfect(AHumanCharacter* DamagedCharacter, ABaseController* DamagedController,
	AMutantCharacter* AttackerCharacter, ABaseController* AttackerController, EMutantState MutantState)
{
	if (DamagedCharacter == nullptr || DamagedController == nullptr || AttackerController == nullptr) return;

	AMutationPlayerState* DamagedState = Cast<AMutationPlayerState>(DamagedController->PlayerState);
	AMutationPlayerState* AttackerState = Cast<AMutationPlayerState>(AttackerController->PlayerState);

	if (DamagedState == nullptr || AttackerState == nullptr) return;

	// 增加攻击者连杀
	AttackerState->AddKillStreak();

	// 增加攻击者怒气值
	AttackerState->SetRage(AttackerState->GetRage() + 2000.f);

	// 击杀日志
	AddKillLog(AttackerState, AttackerCharacter, GetDefault<UDamageTypeMutantInfect>(), DamagedState);

	// 人类感染后突变
	Mutate(DamagedCharacter, DamagedController, ESpawnReason::Infect);
}

// 人类突变为突变体
void AMutationMode::Mutate(ACharacter* Character, AController* Controller, ESpawnReason SpawnReason)
{
	if (AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(Character))
	{
		// 记录当前角色的位置和旋转
		FVector Location = HumanCharacter->GetActorLocation();
		FRotator ActorRotation = Character->GetActorRotation();

		// 人类死亡
		HumanCharacter->MulticastMutationDead(false);
		HumanCharacter->Destroy();

		// 重生为突变体
		AssignTeam(Controller, ETeam::Team2);
		SpawnMutantCharacter(Controller, SpawnReason, Location, ActorRotation);
	}
}

// 突变体选择角色重生
void AMutationMode::SelectMutant(ACharacter* Character, AController* Controller)
{
	if (Character && Controller)
	{
		// 记录当前角色的位置和旋转
		FVector Location = Character->GetActorLocation();
		FRotator ActorRotation = Character->GetActorRotation();

		// 销毁旧角色
		Character->Destroy();

		// 生成新角色
		SpawnMutantCharacter(Controller, ESpawnReason::SelectMutant, Location, ActorRotation);
	}
}

// 突变体受到伤害
void AMutationMode::MutantReceiveDamage(AMutantCharacter* DamagedCharacter, ABaseController* DamagedController,
	float Damage, const UDamageType* DamageType, AController* AttackerController, AActor* DamageCauser)
{
	if (MutationGameState == nullptr) MutationGameState = GetGameState<AMutationGameState>();
	if (MutationGameState == nullptr) return;

	if (DamagedCharacter == nullptr || DamagedController == nullptr || AttackerController == nullptr || DamageCauser == nullptr) return;

	AMutationPlayerState* DamagedState = Cast<AMutationPlayerState>(DamagedController->PlayerState);
	AMutationPlayerState* AttackerState = Cast<AMutationPlayerState>(AttackerController->PlayerState);

	if (DamagedState == nullptr || AttackerState == nullptr) return;

	// 击退受伤者
	if (AProjectileBullet* ProjectileBullet = Cast<AProjectileBullet>(DamageCauser))
	{
		FVector ImpulseVector = ProjectileBullet->GetActorForwardVector();
		ImpulseVector.Z = 0.f;

		DamagedCharacter->MulticastRepel(ImpulseVector * ProjectileBullet->GetImpulse(Damage));
	}

	// 设置受伤者血量
	float TakenDamage = Damage * MutationGameState->GetDamageMul() * DamagedState->GetDamageReceivedMul();
	TakenDamage = FMath::Clamp(TakenDamage, 0.f, DamagedCharacter->GetHealth());
	float Health = DamagedCharacter->GetHealth() - TakenDamage;
	DamagedCharacter->SetHealth(Health);
	DamagedCharacter->MulticastSetHealth(Health, AttackerController);

	// 增加怒气值
	DamagedState->SetRage(DamagedState->GetRage() + TakenDamage);

	// 增加攻击者伤害分数
	if (AttackerState != DamagedState) // 受到跌落伤害时，AttackerController和DamageCauser传的是自己
	{
		AttackerState->AddDamage(TakenDamage);
	}

	// 突变体死亡
	if (DamagedCharacter->GetHealth() <= 0.f)
	{
		if (AttackerState != DamagedState)
		{
			// 增加攻击者连杀
			AttackerState->AddKillStreak();
		}

		// 击杀日志
		AddKillLog(AttackerState, DamageCauser, DamageType, DamagedState);

		// 处理突变体死亡流程
		bool bKilledByMelee = false;
		if (const UDamageTypeBase* DamageTypeBase = Cast<UDamageTypeBase>(DamageType))
		{
			if (DamageTypeBase->DamageType == EDamageCauserType::Melee)
			{
				bKilledByMelee = true;
				DamagedState->bKilledByMelee = true;
				MutationGameState->EndRoundIfAllBeKilledByMelee();
			}
		}
		DamagedCharacter->MulticastDead(bKilledByMelee);
	}
}

// 突变体重生
void AMutationMode::MutantRespawn(ACharacter* Character, ABaseController* BaseController, bool bKilledByMelee)
{
	if (Character == nullptr || BaseController == nullptr) return;

	if (!bKilledByMelee)
	{
		Character->Destroy();
		SpawnMutantCharacter(BaseController, ESpawnReason::Respawn);
	}
	// 被刀死不能复活，进入观察视角
	else
	{
		BaseController->SetPlayerSpectate();
	}
}

// 生成突变体角色
void AMutationMode::SpawnMutantCharacter(AController* Controller, ESpawnReason SpawnReason,
	FVector Location, FRotator ActorRotation, FRotator ViewRotation)
{
	if (Controller == nullptr) return;

	// 获取角色名字
	FString CharacterName;
	if (ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(Controller->PlayerState))
	{
		FString EnumValue = UEnum::GetValueAsString(BasePlayerState->GetMutantCharacterName());
		CharacterName = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
	}

	// 获取角色类
	TObjectPtr<UClass> CharacterClass;
	if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
	{
		FDataRegistryId DataRegistryId(DR_MUTANT_CHARACTER_MAIN, FName(CharacterName));
		if (const FMutantCharacterMain* MutantCharacterMain = DRSubsystem->GetCachedItem<FMutantCharacterMain>(DataRegistryId))
		{
			CharacterClass = MutantCharacterMain->MutantCharacterClass;
		}
	}
	if (CharacterClass == nullptr) return;

	// 如果是突变体重生，使用出生点的位置和旋转
	if (SpawnReason == ESpawnReason::Respawn)
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

	MutantCharacter->SpawnReason = SpawnReason;

	Controller->Possess(MutantCharacter);

	// 如果是突变体重生，重置俯仰
	if (SpawnReason == ESpawnReason::Respawn)
	{
		Controller->ClientSetRotation(MutantCharacter->GetActorRotation());
	}
}

// 生成补给箱
void AMutationMode::SpawnPickups()
{
	// 寻找补给箱重生点
	if (PickupStartPoints.IsEmpty())
	{
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			if (It->PlayerStartTag == FName("Pickup"))
			{
				PickupStartPoints.Add(*It);
			}
		}
	}

	// 随机选3个重生点
	auto SelectedStartPoints = PickupStartPoints;
	while (SelectedStartPoints.Num() > 3)
	{
		int32 RandomIndex = FMath::RandRange(0, SelectedStartPoints.Num() - 1);
		SelectedStartPoints.RemoveAt(RandomIndex);
	}

	for (int i = 0; i < SelectedStartPoints.Num(); ++i)
	{
		if (SelectedStartPoints[i] == nullptr) continue;
		// 生成
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		GetWorld()->SpawnActor<APickup>(
			PickupClasses[FMath::RandRange(0, PickupClasses.Num() - 1)],
			SelectedStartPoints[i]->GetActorLocation(),
			SelectedStartPoints[i]->GetActorRotation(),
			SpawnParams
		);
	}
}
