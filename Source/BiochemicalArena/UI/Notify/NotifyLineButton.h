#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "NotifyLineButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UNotifyLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Notify;

};
