#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "Throwing.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AThrowing : public AEquipment
{
	GENERATED_BODY()

public:
	AThrowing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* ThrowMontage_C;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* ThrowMontage_E;
	virtual void ThrowOut();

	virtual void ManualDestroy();

};
