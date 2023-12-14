#include "BaseController.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "Blueprint/UserWidget.h"

void ABaseController::BeginPlay()
{
	Super::BeginPlay();

	AddHUDContainer();
}

void ABaseController::AddHUDContainer()
{
	if (!IsLocalController()) return;
	UObject* HUDContainerClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/HUD/W_HUDContainer.W_HUDContainer_C'"));
	if (HUDContainerClass) HUDContainer = CreateWidget<UHUDContainer>(this, Cast<UClass>(HUDContainerClass));
	if (HUDContainer)
	{
		HUDContainer->AddToViewport();
	}
}

void ABaseController::ShowScoreboard(bool bIsShow)
{
	if (HUDContainer && HUDContainer->Scoreboard)
	{
		if (bIsShow) HUDContainer->Scoreboard->SetVisibility(ESlateVisibility::Visible);
		else HUDContainer->Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABaseController::ShowPauseMenu()
{
	if (HUDContainer) HUDContainer->ShowPauseMenu();
}
