#include "Login.h"

#include "Auth.h"
#include "CommonTextBlock.h"
#include "Connect.h"
#include "ServiceManager.h"
#include "BiochemicalArena/PlayerControllers/LoginController.h"
#include "BiochemicalArena/System/PlayerSubsystem.h"
#include "Common/CommonButton.h"

void ULogin::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	LoginStatus->SetText(FText::FromString("Not logged in"));

	FString CommandLineToken;
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD="), CommandLineToken);
	LoginButton->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, ELoginType::ExchangeCode, FString(""), CommandLineToken);

#if UE_BUILD_DEVELOPMENT
	Login1Button->SetVisibility(ESlateVisibility::Visible);
	Login2Button->SetVisibility(ESlateVisibility::Visible);
	Login3Button->SetVisibility(ESlateVisibility::Visible);

	Login1Button->ButtonText->SetText(FText::FromString("dust9923"));
	Login1Button->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, ELoginType::DevAuth, FString("127.0.0.1:6000"), FString("dust9923"));

	Login2Button->ButtonText->SetText(FText::FromString("dev10001"));
	Login2Button->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, ELoginType::DevAuth, FString("127.0.0.1:6000"), FString("dev10001"));

	Login3Button->ButtonText->SetText(FText::FromString("dev10002"));
	Login3Button->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, ELoginType::DevAuth, FString("127.0.0.1:6000"), FString("dev10002"));
#endif
}

UWidget* ULogin::NativeGetDesiredFocusTarget() const
{
	return LoginButton;
}

void ULogin::OnLoginButtonClicked(ELoginType LoginType, FString ID, FString Token)
{
	ServiceManager = UServiceManager::GetServiceManager();
	if (ServiceManager)
	{
		if (ServiceManager->InitEOS())
		{
			Auth = ServiceManager->GetAuth();
			if (Auth)
			{
				if (!Auth->OnLoginComplete.IsBoundToObject(this)) Auth->OnLoginComplete.AddUObject(this, &ThisClass::OnLoginComplete);
			}

			Connect = ServiceManager->GetConnect();
			if (Connect)
			{
				if (!Connect->OnConnectComplete.IsBoundToObject(this)) Connect->OnConnectComplete.AddUObject(this, &ThisClass::OnConnectComplete);
			}

			if (LoginController == nullptr) LoginController = Cast<ALoginController>(GetOwningPlayer());
			if (LoginController)
			{
				UPlayerSubsystem* PlayerSubsystem = ULocalPlayer::GetSubsystem<UPlayerSubsystem>(LoginController->GetLocalPlayer());
				if (PlayerSubsystem)
				{
					PlayerSubsystem->Login(LoginType, ID, Token);

					LoginStatus->SetText(FText::FromString("Logging..."));
					LoginButton->SetIsEnabled(false);
				}
			}
		}
		else
		{
			LoginButton->SetIsEnabled(true);
			LoginStatus->SetText(FText::FromString("Init EOSSDK failed!"));
		}
	}
}

void ULogin::OnLoginComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		LoginStatus->SetText(FText::FromString("Connecting..."));

		if (Connect)
		{
			Connect->Connect();
		}
	}
	else
	{
		LoginButton->SetIsEnabled(true);
		LoginStatus->SetText(FText::FromString("Login failed!"));
	}
}

void ULogin::OnConnectComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		LoginStatus->SetText(FText::FromString("Redirecting..."));
	}
	else
	{
		LoginButton->SetIsEnabled(true);
		LoginStatus->SetText(FText::FromString("Connect failed!"));
	}
}
