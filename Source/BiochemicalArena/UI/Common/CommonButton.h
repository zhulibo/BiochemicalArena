#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCommonButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* ButtonText;

	UPROPERTY()
	FString Name;
	
};
