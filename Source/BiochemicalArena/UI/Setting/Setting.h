#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Setting.generated.h"

UCLASS()
class BIOCHEMICALARENA_API USetting : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY()
	class ABaseController* BaseController;
	UFUNCTION()
	void OnBackButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* BackButton;

};
