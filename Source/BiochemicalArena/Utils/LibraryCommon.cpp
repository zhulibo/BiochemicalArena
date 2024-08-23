#include "LibraryCommon.h"

#include "Kismet/KismetMathLibrary.h"

int32 ULibraryCommon::GetBloodParticleCount(float Damage)
{
	return  UKismetMathLibrary::MapRangeClamped(Damage, 40, 200, 3, 10);
}
