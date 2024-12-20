#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "LoadoutTab.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULoadoutTab : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditInstanceOnly)
	class UCommonActivatableWidgetSwitcher* LoadoutSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TabButtonContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCommonButton> TabButtonClass;
	UPROPERTY(EditInstanceOnly)
	TArray<class ULoadoutContent*> TabContents;
	void LinkSwitcher();

};
