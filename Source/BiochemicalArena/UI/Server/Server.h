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

	void OnServerReFreshButtonClicked();
	void OnServerResetButtonClicked();
	void OnServerCreateButtonClicked();
	void OnServerLineButtonClicked(class UServerLineButton* ServerLineButton);

	UPROPERTY()
	FText ServerName;
	UFUNCTION()
	void OnServerNameInputTextChanged(const FText& Text);

private:
	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ServerLineButtonContainer;
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* ServerCreateButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerReFreshButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerResetButton;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameInput;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UServerLineButton> ServerLineButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULobby> LobbyClass;

};
