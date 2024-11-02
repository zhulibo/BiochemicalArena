#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NotifyLayout.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UNotifyLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void AddNotify(const FColor DisplayColor, const FText& Msg);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* NotifyContainer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UNotifyLineButton> NotifyLineButtonClass;

};
