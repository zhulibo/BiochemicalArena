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

	UPROPERTY(EditInstanceOnly)
	class UCommonActivatableWidgetSwitcher* BagSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TabButtonContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCommonButton> TabButtonClass;
	UPROPERTY(EditInstanceOnly)
	TArray<class UBagContent*> TabContents;
	void LinkSwitcher();

};
