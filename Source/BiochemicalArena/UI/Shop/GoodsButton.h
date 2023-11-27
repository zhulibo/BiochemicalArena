#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GoodsButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UGoodsButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* GoodsNameText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* PriceText;

};
