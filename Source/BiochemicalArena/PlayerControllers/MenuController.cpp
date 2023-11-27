#include "MenuController.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "Blueprint/UserWidget.h"

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	AddMenuContainer();
}

void AMenuController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMenuController::AddMenuContainer()
{
	if (MenuContainerClass)
	{
		MenuContainer = CreateWidget<UMenuContainer>(this, MenuContainerClass);
		if (MenuContainer)
		{
			MenuContainer->AddToViewport();
			MenuContainer->ActivateWidget();
			FInputModeUIOnly InputModeData;
			SetInputMode(InputModeData);
		}
	}
}
