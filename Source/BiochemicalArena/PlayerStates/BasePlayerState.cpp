#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TeamType.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"

ABasePlayerState::ABasePlayerState()
{
	Team = ETeam::NoTeam;
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, Defeat);
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
			SpawnCharacterName = StorageSubsystem->StorageCache->Character;
			ServerSetSpawnCharacterName(StorageSubsystem->StorageCache->Character);
		}
	}
}

void ABasePlayerState::SetTeam(ETeam TempTeam)
{
	Team = TempTeam;
}

void ABasePlayerState::OnRep_Team()
{
}

void ABasePlayerState::ServerSetSpawnCharacterName_Implementation(const FString& TempSpawnCharacterName)
{
	SpawnCharacterName = TempSpawnCharacterName;
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
