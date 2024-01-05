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

	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* ThrowMontage;
	virtual void ThrowOut();

	virtual void ManualDestroy();

};
