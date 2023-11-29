#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "PlayerLine.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UPlayerLine : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;

private:

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* KickButton;

};
