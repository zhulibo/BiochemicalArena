#include "MutationPlayerState.h"

#include "BiochemicalArena/Abilities/AttributeSetBase.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "Net/UnrealNetwork.h"

void AMutationPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, Rage);
	DOREPLIFETIME(ThisClass, bKilledByMelee);
}

void AMutationPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AMutationPlayerState::Reset()
{
	Super::Reset();

	SetRage(0.f);

	bKilledByMelee = false;
}

void AMutationPlayerState::SetTeam(ETeam TempTeam)
{
	Super::SetTeam(TempTeam);

	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		MutationController->OnTeamChange.Broadcast(TempTeam);
	}
}

void AMutationPlayerState::OnRep_Team()
{
	Super::OnRep_Team();

	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		MutationController->OnTeamChange.Broadcast(Team);

		// OnControllerReady > InitHUD依赖于Team，OnRep_Team后主动调一下InitHUD。
		MutationController->InitHUD();
	}
}

void AMutationPlayerState::AddDamage(float TempDamage)
{
	Super::AddDamage(TempDamage);

	Show1000DamageUI();
}

void AMutationPlayerState::OnRep_Damage()
{
	Super::OnRep_Damage();

	Show1000DamageUI();
}

void AMutationPlayerState::Show1000DamageUI()
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		if (Damage - BaseDamage >= 1000)
		{
			BaseDamage = FMath::FloorToInt(Damage / 1000.f) * 1000;

			MutationController->OnCause1000Damage.Broadcast();
		}
	}
}

void AMutationPlayerState::SetRage(float TempRage)
{
	Rage = TempRage;

	if (AttributeSetBase)
	{
		if (Rage >= 8000.f && Rage < 14000.f)
		{
			if (GetCharacterLevel() < 2.f)
			{
				AttributeSetBase->SetCharacterLevel(2.f);
			}
		}
		else if (Rage >= 14000.f)
		{
			if (GetCharacterLevel() < 3.f)
			{
				AttributeSetBase->SetCharacterLevel(3.f);
			}
		}
	}

	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		MutationController->SetHUDRage(Rage);
	}
}

void AMutationPlayerState::OnRep_Rage()
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		MutationController->SetHUDRage(Rage);
	}
}
