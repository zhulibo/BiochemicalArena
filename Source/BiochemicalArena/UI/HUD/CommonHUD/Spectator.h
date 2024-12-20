#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "Spectator.generated.h"

UCLASS()
class BIOCHEMICALARENA_API USpectator : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SwitchPerspectiveButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ViewPrevButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ViewNextButton;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* CurPlayer;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class ABaseController* BaseController;

	void OnViewTargetChange(AActor* ViewTarget);

};
