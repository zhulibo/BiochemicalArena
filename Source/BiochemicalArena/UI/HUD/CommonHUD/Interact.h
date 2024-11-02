#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Interact.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInteract : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar;

	void OnInteractStarted();
	void OnInteractEnded();

};
