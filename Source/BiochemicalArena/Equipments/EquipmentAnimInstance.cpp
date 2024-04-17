#include "EquipmentAnimInstance.h"

#include "Equipment.h"

void UEquipmentAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// if (Equipment == nullptr) Equipment = Cast<AEquipment>(TryGetPawnOwner());
}
