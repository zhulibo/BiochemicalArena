#include "HumanController.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/HUD/TeamDeadMatch.h"
#include "Kismet/GameplayStatics.h"

void AHumanController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (HUDContainer) HUDContainer->TeamDeadMatch->SetVisibility(ESlateVisibility::Visible);
	}
}

void AHumanController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController() && !bHasInitDefaultHUD)
	{
		InitDefaultHUD();
	}
}

void AHumanController::SetHasInitDefaultHUD(bool bHasInit)
{
	bHasInitDefaultHUD = bHasInit;
}

void AHumanController::InitDefaultHUD()
{
	if (TeamDeadMatchState == nullptr) TeamDeadMatchState = Cast<ATeamDeadMatchState>(UGameplayStatics::GetGameState(GetWorld()));
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetPawn());

	if (TeamDeadMatchState && HumanCharacter)
	{
		SetHUDHealth(HumanCharacter->GetMaxHealth());
		SetHUDTeamScore(TeamDeadMatchState->GetTeamScore(ETeam::Team1), ETeam::Team1);
		SetHUDTeamScore(TeamDeadMatchState->GetTeamScore(ETeam::Team2), ETeam::Team2);

		bHasInitDefaultHUD = true;
	}
}

void AHumanController::SetHUDHealth(float Health)
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
		HUDContainer->TeamDeadMatch->Ammo->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDCarriedAmmo(int32 Ammo)
{
	if (HUDContainer)
	{
		HUDContainer->TeamDeadMatch->CarriedAmmo->SetText(FText::AsNumber(Ammo));
	}
}

void AHumanController::SetHUDTeamScore(float Score, ETeam Team)
{
	if (HUDContainer == nullptr) return;
	switch (Team)
	{
	case ETeam::Team1:
		HUDContainer->TeamDeadMatch->Team1Score->SetText(FText::AsNumber(Score));
		break;
	case ETeam::Team2:
		HUDContainer->TeamDeadMatch->Team2Score->SetText(FText::AsNumber(Score));
		break;
	}
}
