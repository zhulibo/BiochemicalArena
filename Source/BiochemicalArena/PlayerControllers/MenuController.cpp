#include "MenuController.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "Blueprint/UserWidget.h"

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	AddMenuContainer();

	// 初始化默认设置
	UStorageSubsystem* StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->InitDefaultSetting();
	}
}

void AMenuController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMenuController::AddMenuContainer()
{
	if (IsLocalController() && MenuContainerClass)
	{
		MenuContainer = CreateWidget<UMenuContainer>(this, MenuContainerClass);
		if (MenuContainer)
		{
			MenuContainer->AddToViewport();
			MenuContainer->ActivateWidget();
			FInputModeUIOnly InputModeData;
			SetInputMode(InputModeData);
			SetShowMouseCursor(true);
		}
	}
}
