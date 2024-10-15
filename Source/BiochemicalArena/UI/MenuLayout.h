#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MenuLayout.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenuLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// 菜单层
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* MenuStack;
	// 模态层
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* ModalStack;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMenu> MenuClass;

};
