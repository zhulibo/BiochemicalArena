#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Login.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULogin : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class ALoginController* LoginController;
	UPROPERTY()
	class UEOSSubsystem* EOSSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* LoginStatus;
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* LoginButton;
	void OnLoginButtonClicked(int32 Type);
	void OnLoginComplete(bool bWasSuccessful);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login1Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login2Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login3Button;

};
