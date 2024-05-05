#include "LoginController.h"
#include "BiochemicalArena/UI/Login.h"
#include "Blueprint/UserWidget.h"

void ALoginController::BeginPlay()
{
	Super::BeginPlay();

	AddContainer();
}

void ALoginController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ALoginController::AddContainer()
{
	if (IsLocalController() && LoginClass)
	{
		Login = CreateWidget<ULogin>(this, LoginClass);
		if (Login)
		{
			Login->AddToViewport(999);
			Login->ActivateWidget();
			FInputModeUIOnly InputModeData;
			SetInputMode(InputModeData);
			SetShowMouseCursor(true);
		}
	}
}
