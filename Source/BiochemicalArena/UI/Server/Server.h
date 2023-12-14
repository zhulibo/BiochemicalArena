#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "EOS.h"
#include "Server.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UServer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	FText ServerName;
	UFUNCTION()
	void OnServerNameInputTextChanged(const FText& Text);

	void OnServerReFreshButtonClicked();
	void OnServerResetButtonClicked();
	void OnServerCreateButtonClicked();
	void OnLogin1ButtonClicked();
	void OnLogin2ButtonClicked();
	void OnServerLineButtonClicked(class UServerLineButton* ServerLineButton);

	void OnCreateLobbyComplete(bool bWasSuccessful);
	void OnFindLobbyComplete(bool bWasSuccessful, const TArray<TSharedRef<const FLobby>>& Lobbies);
	void OnJoinLobbyComplete(bool bWasSuccessful);
	void OnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded);
	void OnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested);

private:
	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ServerLineButtonContainer;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameInput;
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* ServerReFreshButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerResetButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerCreateButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login1Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login2Button;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UServerLineButton> ServerLineButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULobby> LobbyClass;

	UPROPERTY()
	UEOS* EOS;

};
