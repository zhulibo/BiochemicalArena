#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "TabButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* TabButtonText;

};
