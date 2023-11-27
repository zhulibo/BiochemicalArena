#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Server.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UServer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void NativeConstruct() override;

protected:
	void AddServerListItem();

	void OnServerLineButtonClicked(class UServerLineButton* ServerLineButton);
	void OnServerReFreshButtonClicked();
	void OnServerResetButtonClicked();

	UPROPERTY()
	FText ServerName;
	UFUNCTION()
	void OnServerNameInputTextChanged(const FText& Text);

private:
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ServerLineButtonContainer;
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* ServerReFreshButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerResetButton;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameInput;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UServerLineButton> ServerLineButtonClass;

};
