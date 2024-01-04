#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "BagTab.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBagTab : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditInstanceOnly, Category = "UI")
	class UCommonActivatableWidgetSwitcher* BagSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TabButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCommonButton> TabButtonClass;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	TArray<class UCommonActivatableWidget*> TabContent;
	void LinkSwitcher();

};
