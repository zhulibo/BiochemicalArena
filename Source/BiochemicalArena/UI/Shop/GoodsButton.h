#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "GoodsButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UGoodsButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* GoodsNameText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* PriceText;

};
