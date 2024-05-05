#include "PlayerSubsystem.h"

#include "BiochemicalArena/BiochemicalArena.h"

UPlayerSubsystem::UPlayerSubsystem()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		EOSSubsystem = LocalPlayer->GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			EOSSubsystem->OnLoginComplete.AddUObject(this, &ThisClass::OnLoginComplete);
			EOSSubsystem->OnLoginStatusChanged.AddUObject(this, &ThisClass::OnLoginStatusChanged);
		}
	}
}

void UPlayerSubsystem::Login(int32 Type)
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (EOSSubsystem == nullptr) EOSSubsystem = LocalPlayer->GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorWhite, TEXT("Logging..."), false);
			EOSSubsystem->Login(LocalPlayer->GetPlatformUserId(), Type);
		}
	}
}

void UPlayerSubsystem::OnLoginComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorMutate, TEXT("Login success!"), false);
		GetWorld()->ServerTravel("/Game/Maps/Menu?listen");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Login failed!"), false);
	}
}

void UPlayerSubsystem::OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	if (AuthLoginStatusChanged.LoginStatus != ELoginStatus::LoggedIn || AuthLoginStatusChanged.LoginStatus != ELoginStatus::LoggedInReducedFunctionality)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorMain, TEXT("Login status changed!"), false);
		GetWorld()->ServerTravel("/Game/Maps/Login?listen");
	}
}
