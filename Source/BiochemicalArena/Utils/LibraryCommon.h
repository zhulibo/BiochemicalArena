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
	static FString GetNowFormatTime();
	
	static FColor GetProgressColor(double Value, double InRangeA, double InRangeB, FColor InColor, FColor OutColor);

	static FString ObfuscatePlayerName(FString PlayerName, const UObject* Context);
	static FString ObfuscateTextChat(FString Msg, const UObject* Context);
	static FString ObfuscateText(FString Text);

};
