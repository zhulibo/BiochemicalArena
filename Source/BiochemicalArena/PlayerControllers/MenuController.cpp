#include "MenuController.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "Blueprint/UserWidget.h"

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	AddMenu();
}

void AMenuController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMenuController::AddMenu()
{
	if (MenuContainerClass)
	{
		MenuContainer = CreateWidget<UMenuContainer>(this, MenuContainerClass);
		MenuContainer->AddToViewport();
		MenuContainer->ActivateWidget();
		FInputModeUIOnly InputModeData;
		SetInputMode(InputModeData);
	}
}
