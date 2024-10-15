#include "LibraryCommon.h"

#include "Kismet/KismetMathLibrary.h"

int32 ULibraryCommon::GetBloodParticleCount(float Damage)
{
	return  UKismetMathLibrary::MapRangeClamped(Damage, 40, 200, 3, 10);
}

FString ULibraryCommon::GetFormatTime(int32 CountdownTime)
{
	int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
	int32 Seconds = CountdownTime - Minutes * 60;
	return  FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}