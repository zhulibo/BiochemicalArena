#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BagMenuButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBagMenuButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PrimaryEquipmentText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* SecondaryEquipmentText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* MeleeEquipmentText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ThrowingEquipmentText;

};
