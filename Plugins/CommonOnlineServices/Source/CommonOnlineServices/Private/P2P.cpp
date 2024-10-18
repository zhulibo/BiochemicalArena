#include "P2P.h"

#include <eos_sdk.h>
#include <eos_p2p.h>

#include "ServiceManager.h"

UP2P::UP2P()
{
}

void UP2P::QueryNATType()
{
	EOS_HP2P P2PHandle = EOS_Platform_GetP2PInterface(UServiceManager::GetPlatformHandle());

	EOS_P2P_QueryNATTypeOptions Options = {};
	Options.ApiVersion = EOS_P2P_QUERYNATTYPE_API_LATEST;

	EOS_P2P_QueryNATType(P2PHandle, &Options, nullptr, QueryNATTypeCb);
}

void UP2P::QueryNATTypeCb(const EOS_P2P_OnQueryNATTypeCompleteInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("QueryNATTypeCb succeeded!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("QueryNATTypeCb failed!"));
	}
}

void UP2P::GetNATType()
{
	EOS_HP2P P2PHandle = EOS_Platform_GetP2PInterface(UServiceManager::GetPlatformHandle());

	EOS_P2P_GetNATTypeOptions Options = {};
	Options.ApiVersion = EOS_P2P_GETNATTYPE_API_LATEST;

	EOS_ENATType NATType = EOS_ENATType::EOS_NAT_Unknown;
	EOS_EResult Result = EOS_P2P_GetNATType(P2PHandle, &Options, &NATType);

	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_P2P_GetNATType failed with code %d!"), Result);
	}

	UE_LOG(LogTemp, Warning, TEXT("NAT Type: %u"), NATType);
}
