#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/HorizontalBox.h"
#include "BagTab.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBagTab : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void LinkSwitcher();

private:
	UPROPERTY(EditInstanceOnly, Category = "UI")
	UCommonActivatableWidgetSwitcher* BagSwitcher;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	TArray<UCommonActivatableWidget*> TabContent;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* TabButtonContainer;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCommonButton> TabButtonClass;

};
