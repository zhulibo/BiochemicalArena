#pragma once

#include "CoreMinimal.h"
// #include "Online/Commerce.h"
#include "CommonButtonBase.h"
#include "GoodsButton.generated.h"

using namespace UE::Online;

UCLASS()
class BIOCHEMICALARENA_API UGoodsButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* GoodsName;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Price;

	// FOffer Offer;

};
