#include "PlayerSubsystem.h"

#include "Auth.h"
#include "Connect.h"
#include "ServiceManager.h"
#include "BiochemicalArena/BiochemicalArena.h"

UPlayerSubsystem::UPlayerSubsystem()
{
}

void UPlayerSubsystem::Tick(float DeltaTime)
{
	if (ServiceManager)
	{
		ServiceManager->UpdateEOS();
	}
}

void UPlayerSubsystem::Login(ELoginType LoginType, FString ID, FString Token)
{
	ServiceManager = UServiceManager::GetServiceManager();
	if (ServiceManager)
	{
		Auth = ServiceManager->GetAuth();
		if (Auth)
		{
			if (!Auth->OnLoginOut.IsBoundToObject(this)) Auth->OnLoginOut.AddUObject(this, &ThisClass::OnLoginOut);
		}

		Connect = ServiceManager->GetConnect();
		if (Connect)
		{
			if (!Connect->OnConnectComplete.IsBoundToObject(this)) Connect->OnConnectComplete.AddUObject(this, &ThisClass::OnConnectComplete);
			if (!Connect->OnConnectLoginOut.IsBoundToObject(this)) Connect->OnConnectAuthExpiration.AddUObject(this, &ThisClass::OnConnectAuthExpiration);
			if (!Connect->OnConnectLoginOut.IsBoundToObject(this)) Connect->OnConnectLoginOut.AddUObject(this, &ThisClass::OnConnectLoginOut);
		}
	}

	if (Auth)
	{
		Auth->Login(LoginType, ID, Token);
	}
}

void UPlayerSubsystem::OnConnectComplete(bool bWasSuccessful)
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
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Login failed!"), false);
	}
}

void UPlayerSubsystem::OnLoginOut()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("OnLoginOut!"), false);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
		{
			PlayerController->ClientTravel("/Game/Maps/UI_Login", ETravelType::TRAVEL_Absolute);
		}
	}
}

void UPlayerSubsystem::OnConnectAuthExpiration()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("OnConnectAuthExpiration! Try to refreshing."), false);
}

void UPlayerSubsystem::OnConnectLoginOut()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("OnConnectLoginOut!"), false);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
		{
			PlayerController->ClientTravel("/Game/Maps/UI_Login", ETravelType::TRAVEL_Absolute);
		}
	}
}
