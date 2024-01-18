#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TeamType.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/PlayerSubsystem.h"

ABasePlayerState::ABasePlayerState()
{
	Team = ETeam::NoTeam;
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, SpawnCharacterName);
	DOREPLIFETIME(ThisClass, Defeat);
}

void ABasePlayerState::BeginPlay()
{
	Super::BeginPlay();

	BaseController = Cast<ABaseController>(GetOwner());
	if (BaseController && BaseController->IsLocalController())
	{
		UPlayerSubsystem* PlayerSubsystem = BaseController->GetLocalPlayer()->GetSubsystem<UPlayerSubsystem>();
		if (PlayerSubsystem)
		{
			ServerSetSpawnCharacterName(PlayerSubsystem->GetSpawnCharacterName());
		}
	}
}

void ABasePlayerState::SetTeam(ETeam TemTeam)
{
	Team = TemTeam;
}

void ABasePlayerState::OnRep_Team()
{
}

void ABasePlayerState::ServerSetSpawnCharacterName_Implementation(const FString& TemSpawnCharacterName)
{
	SpawnCharacterName = TemSpawnCharacterName;
}

void ABasePlayerState::AddScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
}

void ABasePlayerState::OnRep_Score()
{
	Super::OnRep_Score();
}

void ABasePlayerState::AddDefeat(int32 DefeatAmount)
{
	Defeat += DefeatAmount;
}

void ABasePlayerState::OnRep_Defeat()
{
}
