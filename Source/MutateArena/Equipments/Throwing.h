#pragma once

#include "CoreMinimal.h"
#include "MutateArena/Equipments/Equipment.h"
#include "Throwing.generated.h"

UCLASS()
class MUTATEARENA_API AThrowing : public AEquipment
{
	GENERATED_BODY()

public:
	AThrowing();

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ThrowMontage_C;
	UPROPERTY(EditAnywhere)
	UAnimMontage* ThrowMontage_E;
	virtual void ThrowOut();

};
