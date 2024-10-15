#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LibraryCommon.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULibraryCommon : public UObject
{
	GENERATED_BODY()

public:
	static int32 GetBloodParticleCount(float Damage);
	
	static FString GetFormatTime(int32 CountdownTime);

};
