#include "MutationGameState.h"

#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/MutationPlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Net/UnrealNetwork.h"

AMutationGameState::AMutationGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMutationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DamageMul);
}

void AMutationGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		WatchGameState();
	}
}

void AMutationGameState::WatchGameState()
{
	if (MutationMode == nullptr) MutationMode = Cast<AMutationMode>(GetWorld()->GetAuthGameMode());
	if (MutationMode == nullptr) return;

	// 监视比赛状态
	if (MutationMode->bWatchMatchState)
	{
		if (Team1PlayerStates.Num() + Team2PlayerStates.Num() < 2)
		{
			MutationMode->EndMatch();
		}
	}

	// 监视对局状态
	if (MutationMode->bWatchRoundState)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Team1.Num(): %d, Team2.Num(): %d"), Team1.Num(), Team2.Num());
		if (Team1PlayerStates.IsEmpty() || Team2PlayerStates.IsEmpty())
		{
			MutationMode->EndRound();
		}
	}
}

void AMutationGameState::AddToPlayerStates(ABasePlayerState* BasePlayerState, ETeam Team)
{
	Super::AddToPlayerStates(BasePlayerState, Team);

	CalcDamageMul();

	SetHUDTeamNum(GetPlayerStates(Team).Num(), Team);
}

void AMutationGameState::RemoveFromPlayerStates(ABasePlayerState* BasePlayerState, ETeam Team)
{
	Super::RemoveFromPlayerStates(BasePlayerState, Team);

	CalcDamageMul();

	SetHUDTeamNum(GetPlayerStates(Team).Num(), Team);
}

void AMutationGameState::EndRoundIfAllBeKilledByMelee()
{
	if (MutationMode == nullptr) MutationMode = Cast<AMutationMode>(GetWorld()->GetAuthGameMode());
	if (MutationMode == nullptr) return;

	bool bAllKilledByMelee = true;
	for (int i = 0; i < Team2PlayerStates.Num(); ++i)
	{
		if (AMutationPlayerState* MutationPlayerState = Cast<AMutationPlayerState>(Team2PlayerStates[i]))
		{
			if (MutationPlayerState->bKilledByMelee == false)
			{
				bAllKilledByMelee = false;
				break;
			}
		}
	}
	if (bAllKilledByMelee)
	{
		MutationMode->EndRound();
	}
}

void AMutationGameState::CalcDamageMul()
{
	if (MatchState != MatchState::InProgress) return;
	if (Team1PlayerStates.IsEmpty() || Team2PlayerStates.IsEmpty()) return;

	float PlayerMul = Team1PlayerStates.Num() / Team2PlayerStates.Num();
	float TempDamageMul = 1.f;

	if (PlayerMul <= .5f)
	{
		TempDamageMul = 2.f;
	}
	else if (PlayerMul <= .75f)
	{
		TempDamageMul = 1.5f;
	}
	else if (PlayerMul <= 1.f)
	{
		TempDamageMul = 1.25f;
	}

	// 伤害加成只增不减
	if (TempDamageMul > DamageMul)
	{
		DamageMul = TempDamageMul;
	}

	OnRep_DamageMul();
}

void AMutationGameState::OnRep_DamageMul()
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetWorld()->GetFirstPlayerController());
	if (MutationController)
	{
		MutationController->SetHUDDamageMul(DamageMul);
	}
}

void AMutationGameState::OnRep_Team1PlayerStates()
{
	Super::OnRep_Team1PlayerStates();

	SetHUDTeamNum(Team1PlayerStates.Num(), ETeam::Team1);
}

void AMutationGameState::OnRep_Team2PlayerStates()
{
	Super::OnRep_Team2PlayerStates();

	SetHUDTeamNum(Team2PlayerStates.Num(), ETeam::Team2);
}

void AMutationGameState::SetHUDTeamNum(int32 TeamNum, ETeam Team)
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetWorld()->GetFirstPlayerController());
	if (MutationController)
	{
		MutationController->SetHUDTeamNum(TeamNum, Team);
	}
}
