#pragma once

#include <eos_auth_types.h>

#include "CoreMinimal.h"
#include "Auth.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE(FOnLoginOut);

UENUM(BlueprintType)
enum class ELoginType : uint8
{
	ExchangeCode,
	Password,
	DevAuth,
};

UCLASS()
class COMMONONLINESERVICES_API UAuth : public UObject
{
	GENERATED_BODY()

public:
	UAuth();

	EOS_EpicAccountId EpicAccountId;

	UFUNCTION()
	void Login(ELoginType LoginMode, FString Id, FString Token);
	static void EOS_CALL LoginCb(const EOS_Auth_LoginCallbackInfo* Data);
	FOnLoginComplete OnLoginComplete;

	bool CopyIdToken(EOS_Auth_IdToken** OutIdToken);

	EOS_NotificationId LoginStatusChangedId = EOS_INVALID_NOTIFICATIONID;
	void AddNotifyLoginStatusChanged();
	static void EOS_CALL LoginStatusChangedCb(const EOS_Auth_LoginStatusChangedCallbackInfo* Data);
	FOnLoginOut OnLoginOut;

};
