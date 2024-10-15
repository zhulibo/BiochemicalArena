#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "StorageButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UStorageButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* ButtonText;

};
