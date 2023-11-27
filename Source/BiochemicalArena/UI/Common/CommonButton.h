#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCommonButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* ButtonText;

};
