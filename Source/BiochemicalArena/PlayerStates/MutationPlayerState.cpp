#include "MutationPlayerState.h"

#include "BiochemicalArena/PlayerControllers/MutationController.h"

void AMutationPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AMutationPlayerState::SetTeam(ETeam TempTeam)
{
	Super::SetTeam(TempTeam);

	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		MutationController->OnTeamChange();
	}
}

void AMutationPlayerState::OnRep_Team()
{
	Super::OnRep_Team();

	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		MutationController->OnTeamChange();
	}
}

void AMutationPlayerState::AddDamage(float TempDamage)
{
	Super::AddDamage(TempDamage);

	Show1000DamageIcon();
}

void AMutationPlayerState::OnRep_Damage()
{
	Super::OnRep_Damage();

	Show1000DamageIcon();
}

void AMutationPlayerState::Show1000DamageIcon()
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwner());
	if (MutationController && MutationController->IsLocalController())
	{
		if (Damage - BaseDamage >= 1000)
		{
			BaseDamage = FMath::FloorToInt(Damage / 1000.f) * 1000;

			MutationController->Show1000DamageIcon();
		}
	}
}
