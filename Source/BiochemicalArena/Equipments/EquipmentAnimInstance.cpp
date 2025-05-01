#include "EquipmentAnimInstance.h"

#include "Equipment.h"

UEquipmentAnimInstance::UEquipmentAnimInstance()
{
	// bUseMultiThreadedAnimationUpdate = false;
}

void UEquipmentAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// if (Equipment == nullptr) Equipment = Cast<AEquipment>(TryGetPawnOwner());
}
