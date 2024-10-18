#include "Connect.h"

#include <eos_sdk.h>

#include "Auth.h"
#include "ServiceManager.h"

UConnect::UConnect()
{
}

void UConnect::Connect()
{
	AddNotifyConnectAuthExpiration();
	AddNotifyConnectLoginStatusChanged();

	UAuth* Auth = UServiceManager::GetServiceManager()->GetAuth();
	EOS_Auth_IdToken* IdToken = nullptr;

	if (!Auth->CopyIdToken(&IdToken))
	{
		UE_LOG(LogTemp, Error, TEXT("CopyIdToken failed!"));
		return;
	}

	EOS_Connect_Credentials Credentials;
	Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
	Credentials.Token = IdToken->JsonWebToken;
	Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_EPIC_ID_TOKEN;

	EOS_Connect_LoginOptions Options = {};
	Options.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
	Options.Credentials = &Credentials;
	Options.UserLoginInfo = nullptr;

	EOS_HConnect ConnectHandle = EOS_Platform_GetConnectInterface(UServiceManager::GetPlatformHandle());

	EOS_Connect_Login(ConnectHandle, &Options, nullptr, ConnectCb);
}

void UConnect::ConnectCb(const EOS_Connect_LoginCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetConnect()->ProductUserId = Data->LocalUserId;

		UServiceManager::GetConnect()->OnConnectComplete.Broadcast(true);
	}
	else if (Data && Data->ResultCode == EOS_EResult::EOS_InvalidUser)
	{
		UE_LOG(LogTemp, Error, TEXT("ConnectCb EOS_InvalidUser"));

		// TODO 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ConnectCb failed with code %d!"), Data->ResultCode);

		UServiceManager::GetConnect()->OnConnectComplete.Broadcast(false);
	}
}

void UConnect::AddNotifyConnectAuthExpiration()
{
	if (ConnectAuthExpirationId == EOS_INVALID_NOTIFICATIONID)
	{
		EOS_Connect_AddNotifyAuthExpirationOptions Options = {};
		Options.ApiVersion = EOS_CONNECT_ADDNOTIFYAUTHEXPIRATION_API_LATEST;

		EOS_HConnect ConnectHandle = EOS_Platform_GetConnectInterface(UServiceManager::GetPlatformHandle());

		ConnectAuthExpirationId = EOS_Connect_AddNotifyAuthExpiration(ConnectHandle, &Options, nullptr, ConnectAuthExpirationCb);
	}
}

void UConnect::ConnectAuthExpirationCb(const EOS_Connect_AuthExpirationCallbackInfo* Data)
{
	if (UServiceManager::GetConnect() == nullptr) return;

	UServiceManager::GetConnect()->OnConnectAuthExpiration.Broadcast();

	UServiceManager::GetConnect()->Connect();
}

void UConnect::AddNotifyConnectLoginStatusChanged()
{
	if (ConnectLoginStatusChangedId == EOS_INVALID_NOTIFICATIONID)
	{
		EOS_Connect_AddNotifyLoginStatusChangedOptions Options = {};
		Options.ApiVersion = EOS_CONNECT_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST;

		EOS_HConnect ConnectHandle = EOS_Platform_GetConnectInterface(UServiceManager::GetPlatformHandle());

		ConnectLoginStatusChangedId = EOS_Connect_AddNotifyLoginStatusChanged(ConnectHandle, &Options, nullptr, ConnectLoginStatusChangedCb);
	}
}

void UConnect::ConnectLoginStatusChangedCb(const EOS_Connect_LoginStatusChangedCallbackInfo* Data)
{
	if (Data == nullptr || UServiceManager::GetConnect() == nullptr) return;

	if (Data->PreviousStatus == EOS_ELoginStatus::EOS_LS_LoggedIn && Data->CurrentStatus == EOS_ELoginStatus::EOS_LS_NotLoggedIn)
	{
		UServiceManager::GetConnect()->OnConnectLoginOut.Broadcast();
	}
}
