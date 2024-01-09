#include "MenuController.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "Blueprint/UserWidget.h"

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	AddMenuContainer();

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnLoginComplete.AddUObject(this, &ThisClass::OnLoginComplete);
		EOSSubsystem->OnLoginStatusChanged.AddUObject(this, &ThisClass::OnLoginStatusChanged);
	}

	// Login();
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

void AMenuController::Login()
{
	if (EOSSubsystem == nullptr) return;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		EOSSubsystem->Login(LocalPlayer->GetPlatformUserId());
	}
}

void AMenuController::Login1()
{
	if (EOSSubsystem == nullptr) return;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		EOSSubsystem->Login(LocalPlayer->GetPlatformUserId(), 1);
	}
}

void AMenuController::Login2()
{
	if (EOSSubsystem == nullptr) return;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		EOSSubsystem->Login(LocalPlayer->GetPlatformUserId(), 2);
	}
}

void AMenuController::OnLoginComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Login Success!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Login Failed!"));
	}
}

// TODO 重新登陆后，测试大厅功能
void AMenuController::OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLoginStatusChanged AuthLoginStatusChanged: %d"), AuthLoginStatusChanged.LoginStatus);
	if (AuthLoginStatusChanged.LoginStatus != ELoginStatus::LoggedIn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("LoginStatus Changed, ReLoging..."));
		Login();
	}
}
