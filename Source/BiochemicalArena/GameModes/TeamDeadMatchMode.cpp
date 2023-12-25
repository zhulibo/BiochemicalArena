#include "TeamDeadMatchMode.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"
#include "BiochemicalArena/Weapons/Weapon.h"

void ATeamDeadMatchMode::BeginPlay()
{
	Super::BeginPlay();

	TeamDeadMatchState = GetGameState<ATeamDeadMatchState>();
	HumanCharacterClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Charaters/BP_HumanCharacter.BP_HumanCharacter_C'"));
}

void ATeamDeadMatchMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATeamDeadMatchMode::KillPlayer(AHumanCharacter* KilledCharacter, AHumanController* KilledController,
	AHumanController* AttackerController, AActor* DamageCauser)
{
	AHumanState* AttackerState = AttackerController ? Cast<AHumanState>(AttackerController->PlayerState) : nullptr;
	AHumanState* KilledState = KilledController ? Cast<AHumanState>(KilledController->PlayerState) : nullptr;

	if (KilledCharacter) KilledCharacter->Kill();

	if (KilledState) KilledState->AddDefeat(1);
	if (AttackerState) AttackerState->AddScore(1.f);

	if (TeamDeadMatchState && AttackerState)
	{
		TeamDeadMatchState->AddTeamScore(AttackerState->Team);
	}

	if (TeamDeadMatchState && AttackerState && DamageCauser && KilledState)
	{
		EWeaponName TemWeaponName = Cast<AWeapon>(DamageCauser->GetOwner())->GetWeaponName(); // Sacrifice checking for performance
		FString WeaponName = UEnum::GetValueAsString(TemWeaponName);
		WeaponName = WeaponName.Right(WeaponName.Len() - WeaponName.Find("::") - 2);
		TeamDeadMatchState->MulticastAddKillLog(AttackerState, WeaponName, KilledState);
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

// 游戏开始
void ATeamDeadMatchMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AHumanController* HumanController = Cast<AHumanController>(*It);
		AHumanCharacter* HumanCharacter = SpawnHumanCharacter(HumanController);
		if (HumanCharacter)
		{
			HumanController->Possess(HumanCharacter);
			AssignPlayerTeam(HumanController);
		}
	}
}

// 中途加入
void ATeamDeadMatchMode::OnPostLogin(AController* NewPlayerController)
{
	Super::OnPostLogin(NewPlayerController);

	if (MatchState == MatchState::InProgress)
	{
		AHumanCharacter* HumanCharacter = SpawnHumanCharacter(NewPlayerController);
		if (HumanCharacter)
		{
			NewPlayerController->Possess(HumanCharacter);
			AssignPlayerTeam(Cast<AHumanController>(NewPlayerController));
		}
	}
}

// 生成角色
AHumanCharacter* ATeamDeadMatchMode::SpawnHumanCharacter(AController* NewPlayerController)
{
	if (HumanCharacterClass == nullptr) return nullptr;
	AActor* StartSpot = ChoosePlayerStart(NewPlayerController);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AHumanCharacter* HumanCharacter = GetWorld()->SpawnActor<AHumanCharacter>(HumanCharacterClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);
	return HumanCharacter;
}

// 分配队伍
void ATeamDeadMatchMode::AssignPlayerTeam(AHumanController* HumanController)
{
	if (TeamDeadMatchState == nullptr || HumanController == nullptr) return;

	AHumanState* HumanState = HumanController->GetPlayerState<AHumanState>();
	if (HumanState)
	{
		if (TeamDeadMatchState->Team1.Num() > TeamDeadMatchState->Team2.Num())
		{
			TeamDeadMatchState->Team2.AddUnique(HumanState);
			HumanState->SetTeam(ETeam::Team2);
		}
		else
		{
			TeamDeadMatchState->Team1.AddUnique(HumanState);
			HumanState->SetTeam(ETeam::Team1);
		}
	}
}
