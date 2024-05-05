#include "Login.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/LoginController.h"
#include "BiochemicalArena/System/PlayerSubsystem.h"
#include "Common/CommonButton.h"

void ULogin::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		if (!EOSSubsystem->OnLoginComplete.IsBoundToObject(this)) EOSSubsystem->OnLoginComplete.AddUObject(this, &ThisClass::OnLoginComplete);
	}

	LoginStatus->SetText(FText::FromString("Not logged in"));

	LoginButton->ButtonText->SetText(FText::FromString("Login"));
	LoginButton->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, 0);

	Login1Button->ButtonText->SetText(FText::FromString("Account 1"));
	Login1Button->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, 1);

	Login2Button->ButtonText->SetText(FText::FromString("Account 2"));
	Login2Button->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, 2);

	Login3Button->ButtonText->SetText(FText::FromString("Account 3"));
	Login3Button->OnClicked().AddUObject(this, &ThisClass::OnLoginButtonClicked, 3);

	// OnLoginButtonClicked(0); // TODO
}

void ULogin::OnLoginButtonClicked(int32 Type)
{
	if (LoginController == nullptr) LoginController = Cast<ALoginController>(GetOwningPlayer());
	if (LoginController)
	{
		UPlayerSubsystem* PlayerSubsystem = ULocalPlayer::GetSubsystem<UPlayerSubsystem>(LoginController->GetLocalPlayer());
		if (PlayerSubsystem)
		{
			LoginStatus->SetText(FText::FromString("Logging..."));
			PlayerSubsystem->Login(Type);
			LoginButton->SetIsEnabled(false);
		}
	}
}

void ULogin::OnLoginComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		LoginStatus->SetText(FText::FromString("Redirecting..."));
	}
	else
	{
		LoginButton->SetIsEnabled(true);
		LoginStatus->SetText(FText::FromString("Login failed!"));
	}
}
