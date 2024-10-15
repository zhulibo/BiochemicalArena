#pragma once

#include <eos_connect_types.h>

#include "CoreMinimal.h"
#include "Connect.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnConnectComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE(FOnConnectAuthExpiration);
DECLARE_MULTICAST_DELEGATE(FOnConnectLoginOut);

UCLASS()
class COMMONONLINESERVICES_API UConnect : public UObject
{
	GENERATED_BODY()

public:
	UConnect();

	EOS_ProductUserId ProductUserId;

	UFUNCTION()
	void Connect();
	static void EOS_CALL ConnectCb(const EOS_Connect_LoginCallbackInfo* Data);
	FOnConnectComplete OnConnectComplete;

	EOS_NotificationId ConnectAuthExpirationId = EOS_INVALID_NOTIFICATIONID;
	void AddNotifyConnectAuthExpiration();
	static void EOS_CALL ConnectAuthExpirationCb(const EOS_Connect_AuthExpirationCallbackInfo* Data);
	FOnConnectAuthExpiration OnConnectAuthExpiration;

	EOS_NotificationId ConnectLoginStatusChangedId = EOS_INVALID_NOTIFICATIONID;
	void  AddNotifyConnectLoginStatusChanged();
	static void EOS_CALL ConnectLoginStatusChangedCb(const EOS_Connect_LoginStatusChangedCallbackInfo* Data);
	FOnConnectLoginOut OnConnectLoginOut;

};
