#include "AnimInstance_Equipment.h"

#include "Equipment.h"

UAnimInstance_Equipment::UAnimInstance_Equipment()
{
	// bUseMultiThreadedAnimationUpdate = false;
}

void UAnimInstance_Equipment::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// if (Equipment == nullptr) Equipment = Cast<AEquipment>(TryGetPawnOwner());
}
