#pragma once

#include <eos_p2p_types.h>

#include "CoreMinimal.h"
#include "P2P.generated.h"

UCLASS()
class COMMONONLINESERVICES_API UP2P : public UObject
{
	GENERATED_BODY()

public:
	UP2P();

	UFUNCTION()
	void QueryNATType();
	static void EOS_CALL QueryNATTypeCb(const EOS_P2P_OnQueryNATTypeCompleteInfo* Data);

	UFUNCTION()
	void GetNATType();

};
