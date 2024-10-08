#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputHuman.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInputHuman : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* HumanMappingContext;

	UPROPERTY(EditAnywhere)
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere)
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere)
	UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere)
	UInputAction* DropAction;
	UPROPERTY(EditAnywhere)
	UInputAction* SwapPrimaryEquipmentAction;
	UPROPERTY(EditAnywhere)
	UInputAction* SwapSecondaryEquipmentAction;
	UPROPERTY(EditAnywhere)
	UInputAction* SwapMeleeEquipmentAction;
	UPROPERTY(EditAnywhere)
	UInputAction* SwapThrowingEquipmentAction;
	UPROPERTY(EditAnywhere)
	UInputAction* SwapLastEquipmentAction;

};
