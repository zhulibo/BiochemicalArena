#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
#include "Lobby.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULobby : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;

	void SetLobbyAttribute();
	void RefreshMapComboBoxOption();

	void RefreshLobbyUI();

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* Team1Container; // TODO Try to use MVVM
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* Team2Container;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerLineButton> PlayerLineButtonClass;
	void AddToPlayerList(TSharedRef<const FLobbyMember> Member);

	void OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined);

	// 大厅属性
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameEditableTextBox;
	FText LastServerName;
	UFUNCTION()
	void OnServerNameCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UPROPERTY(meta = (BindWidget))
	class UCommonComboBox* ModeComboBox;
	UFUNCTION()
	void OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UPROPERTY(meta = (BindWidget))
	UCommonComboBox* MapComboBox;
	UFUNCTION()
	void OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void OnModifyLobbyAttributesComplete(bool bWasSuccessful);
	void OnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged);

	// 成员属性
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SwitchTeamButton;
	UFUNCTION()
	void OnSwitchTeamButtonClicked();
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ReadyButton;
	UFUNCTION()
	void OnReadyButtonClicked();

	void OnModifyLobbyMemberAttributesComplete(bool bWasSuccessful);
	void OnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);

	// 开始游戏
	UPROPERTY(meta = (BindWidget))
	UCommonButton* StartServerButton;
	bool bIsStartingServer = false;
	UFUNCTION()
	void OnStartServerButtonClicked();
	void OnCreateSessionComplete(bool bWasSuccessful);
	void OnAddSessionMemberComplete(bool bWasSuccessful);

	void OnFindSessionComplete(bool bWasSuccessful, const TArray<FOnlineSessionId>& FoundSessionIds);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* JoinServerButton;
	UFUNCTION()
	void OnJoinServerButtonClicked();
	void OnJoinSessionComplete(bool bWasSuccessful);

	// 房主变更事件
	void OnPromoteLobbyMemberComplete(bool bWasSuccessful);
	void OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged);

	// 离开大厅
	UPROPERTY(meta = (BindWidget))
	UCommonButton* BackButton;
	bool bIsExitingLobby = false;
	UFUNCTION()
	void OnBackButtonClicked();
	void OnLeaveLobbyComplete(bool bWasSuccessful);
	void OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft);

	void OnLobbyLeft(const FLobbyLeft& LobbyLeft);

	// 消息
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* MsgEditableTextBox;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* SendMsgButton;
	UFUNCTION()
	void OnSendMsgButtonClicked();

};
