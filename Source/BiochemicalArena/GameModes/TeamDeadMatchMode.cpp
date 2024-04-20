#include "TeamDeadMatchMode.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

void ATeamDeadMatchMode::BeginPlay()
{
	Super::BeginPlay();

	TeamDeadMatchState = GetGameState<ATeamDeadMatchState>();
}

void ATeamDeadMatchMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// 游戏开始
void ATeamDeadMatchMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AHumanController* HumanController = Cast<AHumanController>(*It);
		if (HumanController)
		{
			AssignPlayerTeam(HumanController);

			AHumanCharacter* HumanCharacter = SpawnHumanCharacter(HumanController);
			if (HumanCharacter)
			{
				HumanController->Possess(HumanCharacter);
				HumanController->ClientSetRotation(HumanCharacter->GetActorRotation());
			}
		}
	}
}

// 中途加入
void ATeamDeadMatchMode::OnPostLogin(AController* NewPlayerController)
{
	Super::OnPostLogin(NewPlayerController);

	if (MatchState == MatchState::InProgress)
	{
		AHumanController* HumanController = Cast<AHumanController>(NewPlayerController);
		if (HumanController)
		{
			AssignPlayerTeam(HumanController);

			AHumanCharacter* HumanCharacter = SpawnHumanCharacter(HumanController);
			if (HumanCharacter)
			{
				HumanController->Possess(HumanCharacter);
				HumanController->ClientSetRotation(HumanCharacter->GetActorRotation());
			}
		}
	}
}

// 重生
void ATeamDeadMatchMode::Respawn(ACharacter* KilledCharacter, AController* KilledController)
{
	if (KilledCharacter)
	{
		KilledCharacter->Reset();
		KilledCharacter->Destroy();
	}
	if (KilledController)
	{
		AHumanCharacter* HumanCharacter = SpawnHumanCharacter(KilledController);
		if (HumanCharacter)
		{
			KilledController->Possess(HumanCharacter);
		}
	}
}

// 分配队伍
void ATeamDeadMatchMode::AssignPlayerTeam(AHumanController* HumanController)
{
	if (TeamDeadMatchState == nullptr || HumanController == nullptr) return;

	AHumanState* HumanState = HumanController->GetPlayerState<AHumanState>();
	if (HumanState)
	{
		if (TeamDeadMatchState->GetTeam(ETeam::Team1).Num() > TeamDeadMatchState->GetTeam(ETeam::Team2).Num())
		{
			TeamDeadMatchState->AddToTeam(HumanState, ETeam::Team2);
			HumanState->SetTeam(ETeam::Team2);
		}
		else
		{
			TeamDeadMatchState->AddToTeam(HumanState, ETeam::Team1);
			HumanState->SetTeam(ETeam::Team1);
		}
	}
}

// 生成角色
AHumanCharacter* ATeamDeadMatchMode::SpawnHumanCharacter(AController* NewPlayerController)
{
	if (NewPlayerController == nullptr) return nullptr;

	// 获取角色类
	AHumanState* HumanState = Cast<AHumanState>(NewPlayerController->PlayerState);
	FString SpawnCharacterName = "SAS";
	if (HumanState && !HumanState->GetSpawnCharacterName().IsEmpty())
	{
		SpawnCharacterName = HumanState->GetSpawnCharacterName();
	}

	// double StartTime = FPlatformTime::Seconds();

	FString ClassPath = FString::Printf(TEXT("/Script/Engine.Blueprint'/Game/Characters/Humans/%s/BP_%s.BP_%s_C'"), *SpawnCharacterName, *SpawnCharacterName, *SpawnCharacterName);
	UClass* HumanCharacterClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassPath);

	// double EndTime = FPlatformTime::Seconds();
	// UE_LOG(LogTemp, Warning, TEXT("Load HumanCharacterClass time: %f seconds"), EndTime - StartTime);

	if (HumanCharacterClass == nullptr) return nullptr;

	// 生成角色
	FName PlayerStartTag = HumanState->GetTeam() == ETeam::Team1 ? "Team1" : "Team2";
	AActor* StartSpot = FindRandomPlayerStart(PlayerStartTag);
	if (StartSpot == nullptr) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return GetWorld()->SpawnActor<AHumanCharacter>(
		HumanCharacterClass,
		StartSpot->GetActorLocation(),
		StartSpot->GetActorRotation(),
		SpawnParams
	);
}

// 击杀
void ATeamDeadMatchMode::KillPlayer(AHumanCharacter* KilledCharacter, AHumanController* KilledController,
	AHumanController* AttackerController, AActor* DamageCauser)
{
	AHumanState* AttackerState = AttackerController ? Cast<AHumanState>(AttackerController->PlayerState) : nullptr;
	AHumanState* KilledState = KilledController ? Cast<AHumanState>(KilledController->PlayerState) : nullptr;

	if (KilledState) KilledState->AddDefeat(1);
	if (AttackerState) AttackerState->AddScore(1.f);

	if (TeamDeadMatchState && AttackerState)
	{
		TeamDeadMatchState->AddTeamScore(AttackerState->GetTeam());
	}

	if (TeamDeadMatchState && AttackerState && DamageCauser && KilledState)
	{
		EEquipmentName TempEquipmentName;
		if (AEquipment* DamageCauserEquipment = Cast<AEquipment>(DamageCauser->GetOwner())) // Projectile's owner is Equipment
		{
			TempEquipmentName = DamageCauserEquipment->GetEquipmentName();
		}
		else // Melee's owner is Character
		{
			TempEquipmentName = Cast<AEquipment>(DamageCauser)->GetEquipmentName();
		}
		FString EquipmentName = UEnum::GetValueAsString(TempEquipmentName);
		EquipmentName = EquipmentName.Right(EquipmentName.Len() - EquipmentName.Find("::") - 2);
		TeamDeadMatchState->MulticastAddKillLog(AttackerState, EquipmentName, KilledState);
	}

	if (KilledCharacter)
	{
		KilledCharacter->Kill();
	}
}
