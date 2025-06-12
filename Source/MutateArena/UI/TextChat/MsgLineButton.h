#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "MsgLineButton.generated.h"

UCLASS()
class MUTATEARENA_API UMsgLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Msg;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* MsgTime;

};
