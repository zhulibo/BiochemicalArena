#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "EquipmentButton.h"
#include "BagContent.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBagContent : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UEquipmentButton* Primary;
	UPROPERTY(meta = (BindWidget))
	UEquipmentButton* Secondary;
	UPROPERTY(meta = (BindWidget))
	UEquipmentButton* Melee;
	UPROPERTY(meta = (BindWidget))
	UEquipmentButton* Throwing;

};
