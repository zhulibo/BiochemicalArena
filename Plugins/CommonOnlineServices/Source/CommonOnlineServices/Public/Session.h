#pragma once

#include <eos_base.h>
#include <eos_sessions_types.h>

#include "CoreMinimal.h"
#include "Session.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDestroySessionComplete, bool bWasSuccessful);

UCLASS()
class COMMONONLINESERVICES_API USession : public UObject
{
	GENERATED_BODY()

public:
	USession();
	
	void CreateSession();
	static void EOS_CALL CreateSessionCb(const EOS_Sessions_UpdateSessionCallbackInfo* Data);
	FOnCreateSessionComplete OnCreateSessionComplete;
	
	void DestroySession();
	static void EOS_CALL DestroySessionCb(const EOS_Sessions_DestroySessionCallbackInfo* Data);
	FOnDestroySessionComplete OnDestroySessionComplete;

	void GetSessionDetail();
	
};
