#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "EquipmentButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UEquipmentButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* ButtonText;

};
