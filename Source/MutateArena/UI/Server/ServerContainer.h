#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ServerContainer.generated.h"

UCLASS()
class MUTATEARENA_API UServerContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// 用于添加大厅的层
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* ServerStack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UServer> ServerClass;

protected:
	virtual void NativeConstruct() override;

};
