#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonHierarchicalScrollBox.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "ServerLineButton.h"
#include "Components/EditableTextBox.h"
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

	void OnServerLineButtonClicked(UServerLineButton* ServerLineButton);
	void OnServerReFreshButtonClicked();
	void OnServerResetButtonClicked();

	UPROPERTY()
	FText ServerName;
	UFUNCTION()
	void OnServerNameInputTextChanged(const FText& Text);

private:
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* ServerLineButtonContainer;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerReFreshButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerResetButton;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ServerNameInput;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UServerLineButton> ServerLineButtonClass;

};
