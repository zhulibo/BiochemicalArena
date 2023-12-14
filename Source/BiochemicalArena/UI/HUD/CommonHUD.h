#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonHUD.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCommonHUD : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* AnnouncementText;

};
