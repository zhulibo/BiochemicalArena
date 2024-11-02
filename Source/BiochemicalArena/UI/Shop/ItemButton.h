#pragma once

#include "CoreMinimal.h"
#include "Online/Commerce.h"
#include "CommonButtonBase.h"
#include "ItemButton.generated.h"

using namespace UE::Online;

UCLASS()
class BIOCHEMICALARENA_API UItemButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Price;

	FOffer Offer;

};
