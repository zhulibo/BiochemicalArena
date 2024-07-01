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
			EOSSubsystem->Login(LocalPlayer->GetPlatformUserId(), Type);
		}
	}
}

void UPlayerSubsystem::OnLoginComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
			{
				PlayerController->ClientTravel("/Game/Maps/UI_Menu", ETravelType::TRAVEL_Absolute);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Login failed!"), false);
	}
}

void UPlayerSubsystem::OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	if (AuthLoginStatusChanged.LoginStatus == ELoginStatus::LoggedIn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_MUTANT, TEXT("Login status changed: LoggedIn!"), false);
	}
	else if (AuthLoginStatusChanged.LoginStatus == ELoginStatus::LoggedInReducedFunctionality)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_WHITE, TEXT("Login status changed: LoggedInReducedFunctionality!"), false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_MAIN, TEXT("Login status changed: NotLoggedIn or UsingLocalProfile!"), false);

		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
			{
				PlayerController->ClientTravel("/Game/Maps/UI_Login", ETravelType::TRAVEL_Absolute);
			}
		}
	}
}
