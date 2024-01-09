#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
#include "Server.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UServer : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;

	// 登录
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* Login1Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Login2Button;
	void OnLogin1ButtonClicked();
	void OnLogin2ButtonClicked();

	// 创建大厅
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerCreateButton;
	void OnServerCreateButtonClicked();
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULobby> LobbyClass;
	void OnCreateLobbyComplete(bool bWasSuccessful);

	// 搜索大厅
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameInput;
	UPROPERTY()
	FText ServerName;
	UFUNCTION()
	void OnServerNameInputTextChanged(const FText& Text);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerResetButton;
	void OnServerResetButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerReFreshButton;
	void OnServerReFreshButtonClicked();
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ServerLineButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UServerLineButton> ServerLineButtonClass;
	void OnFindLobbyComplete(bool bWasSuccessful, const TArray<TSharedRef<const FLobby>>& Lobbies);

	// 加入大厅
	void OnServerLineButtonClicked(class UServerLineButton* ServerLineButton);
	void OnJoinLobbyComplete(bool bWasSuccessful);

	// 邀请
	void OnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded);
	void OnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested);

};
