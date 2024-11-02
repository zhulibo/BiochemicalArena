#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "LoginLayout.generated.h"

enum class ECoolLoginType : uint8;

UCLASS()
class BIOCHEMICALARENA_API ULoginLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	
	UPROPERTY()
	class UEOSSubsystem* EOSSubsystem;

	UPROPERTY()
	class ALoginController* LoginController;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* LoginStatus;
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* LoginButton;
	UFUNCTION()
	void OnLoginButtonClicked(ECoolLoginType LoginType, FString Id, FString Token);
	void OnLoginComplete(bool bWasSuccessful);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login1Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login2Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login3Button;

};
