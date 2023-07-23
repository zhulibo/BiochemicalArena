#include "HumanController.h"
#include "BiochemicalArena/HUD/HumanHUD.h"
#include "BiochemicalArena/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"

void AHumanController::BeginPlay()
{
	Super::BeginPlay();

	HumanHUD = Cast<AHumanHUD>(GetHUD());
}

void AHumanController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(InPawn);
	if (HumanCharacter)
	{
		SetHUDHealth(HumanCharacter->GetHealth(), HumanCharacter->GetMaxHealth());
	}
}

void AHumanController::SetHUDHealth(float Health, float MaxHealth)
{
	HumanHUD = HumanHUD == nullptr ? Cast<AHumanHUD>(GetHUD()) : HumanHUD;

	bool bHUDValid = HumanHUD &&
		HumanHUD->CharacterOverlay &&
		HumanHUD->CharacterOverlay->HPBar &&
		HumanHUD->CharacterOverlay->HPValue;
	if (bHUDValid)
	{
		const float HPPercent = Health / MaxHealth;
		HumanHUD->CharacterOverlay->HPBar->SetPercent(HPPercent);
		HumanHUD->CharacterOverlay->HPValue->SetText(FText::FromString(FString::SanitizeFloat(Health)));
	}
}
