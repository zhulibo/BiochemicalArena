#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);
	void SetDisplayText(FString DisplayString);

protected:
	virtual void NativeDestruct() override;

};
