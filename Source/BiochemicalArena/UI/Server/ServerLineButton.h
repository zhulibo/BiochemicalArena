#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "ServerLineButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UServerLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Server;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Map;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Player;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ping;

};
