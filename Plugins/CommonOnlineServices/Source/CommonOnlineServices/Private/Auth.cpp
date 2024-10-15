#include "Auth.h"

#include <eos_sdk.h>
#include <string>

#include "ServiceManager.h"
#include "eos_auth.h"

UAuth::UAuth()
{
	UE_LOG(LogTemp, Warning, TEXT("UAuth"));
}

void UAuth::Login(ELoginType LoginType, FString Id, FString Token)
{
	AddNotifyLoginStatusChanged();

	EOS_Auth_LoginOptions Options = {};
	Options.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
	Options.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_Presence | EOS_EAuthScopeFlags::EOS_AS_FriendsList | EOS_EAuthScopeFlags::EOS_AS_Country;

	EOS_Auth_Credentials Credentials;
	Credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;

	std::string Id2 = TCHAR_TO_UTF8(*Id);
	std::string Token2 = TCHAR_TO_UTF8(*Token);

	switch (LoginType)
	{
	case ELoginType::ExchangeCode:
		Credentials.Token = Token2.c_str();
		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
		break;
	case ELoginType::Password:
		Credentials.Id = Id2.c_str();
		Credentials.Token = Token2.c_str();
		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_Password;
		break;
	case ELoginType::DevAuth:
		Credentials.Id = Id2.c_str();
		Credentials.Token = Token2.c_str();
		Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_Developer;
		break;
	}

	Options.Credentials = &Credentials;
	Options.LoginFlags = 0;

	EOS_HAuth AuthHandle = EOS_Platform_GetAuthInterface(UServiceManager::GetPlatformHandle());

	EOS_Auth_Login(AuthHandle, &Options, nullptr, LoginCb);
}

void UAuth::LoginCb(const EOS_Auth_LoginCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetAuth()->EpicAccountId = Data->LocalUserId;

		UServiceManager::GetAuth()->OnLoginComplete.Broadcast(true);
	}
	else if (Data && Data->ResultCode == EOS_EResult::EOS_InvalidUser)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginCb EOS_InvalidUser"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LoginCb failed with code %d!"), Data->ResultCode);

		UServiceManager::GetAuth()->OnLoginComplete.Broadcast(false);
	}
}

bool UAuth::CopyIdToken(EOS_Auth_IdToken** OutIdToken)
{
	EOS_Auth_CopyIdTokenOptions Options = {};
	Options.ApiVersion = EOS_AUTH_COPYIDTOKEN_API_LATEST;
	Options.AccountId = EpicAccountId;

	EOS_HAuth AuthHandle = EOS_Platform_GetAuthInterface(UServiceManager::GetPlatformHandle());

	EOS_EResult Result = EOS_Auth_CopyIdToken(AuthHandle, &Options, OutIdToken);

	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Auth_CopyIdToken failed with code %d!"), Result);

		return false;
	}

	return true;
}

void UAuth::AddNotifyLoginStatusChanged()
{
	if (LoginStatusChangedId == EOS_INVALID_NOTIFICATIONID)
	{
		EOS_Auth_AddNotifyLoginStatusChangedOptions Options = {};
		Options.ApiVersion = EOS_AUTH_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST;

		EOS_HAuth AuthHandle = EOS_Platform_GetAuthInterface(UServiceManager::GetPlatformHandle());

		LoginStatusChangedId = EOS_Auth_AddNotifyLoginStatusChanged(AuthHandle, &Options, nullptr, LoginStatusChangedCb);
	}
}

void UAuth::LoginStatusChangedCb(const EOS_Auth_LoginStatusChangedCallbackInfo* Data)
{
	if (Data == nullptr) return;

	if (Data->PrevStatus == EOS_ELoginStatus::EOS_LS_LoggedIn && Data->CurrentStatus == EOS_ELoginStatus::EOS_LS_NotLoggedIn)
	{
		UServiceManager::GetAuth()->OnLoginOut.Broadcast();
	}
}
