#include "PlayerSubsystem.h"

UPlayerSubsystem::UPlayerSubsystem()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		EOSSubsystem = LocalPlayer->GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			EOSSubsystem->OnLoginComplete.AddUObject(this, &ThisClass::OnLoginComplete);
			EOSSubsystem->OnLoginStatusChanged.AddUObject(this, &ThisClass::OnLoginStatusChanged);

			// Login(0); // TODO
		}
	}
}

void UPlayerSubsystem::Login(int32 Type)
{
	if (EOSSubsystem == nullptr) return;
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Silver, TEXT("Logging in..."));

		EOSSubsystem->Login(LocalPlayer->GetPlatformUserId(), Type);
	}
}

void UPlayerSubsystem::OnLoginComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Login success!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Login failed!"));
	}
}

void UPlayerSubsystem::OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	if (AuthLoginStatusChanged.LoginStatus != ELoginStatus::LoggedIn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Login status changed!"));

		Login(); // TODO 重新登陆后，测试大厅功能
	}
}
