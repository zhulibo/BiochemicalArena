#include "MenuController.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "BiochemicalArena/UI/Server/EOS.h"
#include "Blueprint/UserWidget.h"

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	AddMenuContainer();

	EOS = GetGameInstance()->GetSubsystem<UEOS>();
	if (EOS)
	{
		EOS->OnLoginComplete.AddUObject(this, &ThisClass::OnLoginComplete);
		EOS->OnLoginStatusChanged.AddUObject(this, &ThisClass::OnLoginStatusChanged);
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
	if (EOS == nullptr) return;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		FPlatformUserId PlatformUserId = LocalPlayer->GetPlatformUserId();
		if (PlatformUserId)
		{
			EOS->Login(PlatformUserId);
		}
	}
}

void AMenuController::Login1()
{
	if (EOS == nullptr) return;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		FPlatformUserId PlatformUserId = LocalPlayer->GetPlatformUserId();
		if (PlatformUserId)
		{
			EOS->Login(PlatformUserId, 1);
		}
	}
}

void AMenuController::Login2()
{
	if (EOS == nullptr) return;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		FPlatformUserId PlatformUserId = LocalPlayer->GetPlatformUserId();
		if (PlatformUserId)
		{
			EOS->Login(PlatformUserId, 2);
		}
	}
}

void AMenuController::OnLoginComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLoginComplete bWasSuccessful: %d"), bWasSuccessful);
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Login Success!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Login Failed!"));
	}
}

// TODO 重新登陆后，测试大厅功能
void AMenuController::OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLoginStatusChanged AuthLoginStatusChanged: %d"), AuthLoginStatusChanged.LoginStatus);
	if (AuthLoginStatusChanged.LoginStatus != ELoginStatus::LoggedIn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("LoginStatus Changed, ReLoging..."));
		Login();
	}
}
