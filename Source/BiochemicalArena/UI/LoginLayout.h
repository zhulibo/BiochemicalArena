#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "LoginLayout.generated.h"

enum class ELoginType : uint8;

UCLASS()
class BIOCHEMICALARENA_API ULoginLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class ALoginController* LoginController;
	UPROPERTY()
	class UServiceManager* ServiceManager;
	UPROPERTY()
	class UAuth* Auth;
	UPROPERTY()
	class UConnect* Connect;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* LoginStatus;
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* LoginButton;
	UFUNCTION()
	void OnLoginButtonClicked(ELoginType LoginType, FString ID, FString Token);
	void OnLoginComplete(bool bWasSuccessful);
	void OnConnectComplete(bool bWasSuccessful);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login1Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login2Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login3Button;

};
