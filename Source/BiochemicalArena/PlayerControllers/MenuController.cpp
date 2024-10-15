#include "MenuController.h"
#include "BiochemicalArena/UI/MenuLayout.h"
#include "Blueprint/UserWidget.h"

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	AddMenuLayout();
}

void AMenuController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMenuController::AddMenuLayout()
{
	if (IsLocalController() && MenuLayoutClass)
	{
		MenuLayout = CreateWidget<UMenuLayout>(this, MenuLayoutClass);
		if (MenuLayout)
		{
			MenuLayout->AddToViewport();
			MenuLayout->ActivateWidget();
			FInputModeUIOnly InputModeData;
			SetInputMode(InputModeData);
			SetShowMouseCursor(true);
		}
	}
}
