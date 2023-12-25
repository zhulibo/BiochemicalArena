#include "HumanController.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/HUD/TeamDeadMatch.h"
#include "BiochemicalArena/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

void AHumanController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		HandleClientServerDelta();
		RequestServerMatchState();

		if (HUDContainer) HUDContainer->TeamDeadMatch->SetVisibility(ESlateVisibility::Visible);

		TeamDeadMatchState = Cast<ATeamDeadMatchState>(UGameplayStatics::GetGameState(GetWorld()));
	}
}

void AHumanController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		InitDefaultHUD();
	}
}

void AHumanController::InitDefaultHUD()
{
	if (bHasInitDefaultHUD) return;

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetPawn());
	if (HumanState == nullptr) HumanState = GetPlayerState<AHumanState>();

	if (HumanCharacter &&
		HumanCharacter->GetCombat() &&
		HumanCharacter->GetCombat()->GetCurrentWeapon() &&
		HumanState)
	{
		SetHUDAmmo(HumanCharacter->GetCombat()->GetCurrentWeapon()->GetAmmo());
		SetHUDCarriedAmmo(HumanCharacter->GetCombat()->GetCurrentWeapon()->GetCarriedAmmo());
		SetHUDHealth(HumanCharacter->GetHealth(), HumanCharacter->GetMaxHealth());
		SetHUDTeamScore(TeamDeadMatchState->Team1Score, 1);
		SetHUDTeamScore(TeamDeadMatchState->Team2Score, 2);

		bHasInitDefaultHUD = true;
	}
}

void AHumanController::SetHUDHealth(float Health, float MaxHealth)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->Health->SetText(FText::AsNumber(Health));
	}
}

void AHumanController::SetHUDAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->WeaponAmmo->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDCarriedAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->CarriedAmmo->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDTeamScore(float Score, int32 Team)
{
	if (HUDContainer == nullptr) return;
	if (Team == 1)
	{
		HUDContainer->TeamDeadMatch->Team1Score->SetText(FText::AsNumber(Score));
	}
	else if (Team == 2)
	{
		HUDContainer->TeamDeadMatch->Team2Score->SetText(FText::AsNumber(Score));
	}
}
