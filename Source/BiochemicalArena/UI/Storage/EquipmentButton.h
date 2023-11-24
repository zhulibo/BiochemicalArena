#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "EquipmentButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UEquipmentButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ButtonText;

};
