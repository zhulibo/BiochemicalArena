#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
#include "Lobby.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULobby : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void OnCreateInit();
	void OnJoinInit();

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* Team1Container;
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* Team2Container;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerLineButton> PlayerLineButtonClass;
	void InitPlayerList();
	void AddToPlayerList(TSharedRef<const FLobbyMember> Member);

	void OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined);

	// 大厅属性
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameEditableTextBox;
	UFUNCTION()
	void OnServerNameChanged(const FText& Text);
	UPROPERTY(meta = (BindWidget))
	class UCommonComboBox* ModeComboBox;
	UFUNCTION()
	void OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UPROPERTY(meta = (BindWidget))
	UCommonComboBox* MapComboBox;
	UFUNCTION()
	void OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void ModifyLobbyAttributes(const FString& Key, const FString& Value); // TODO
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

	void ModifyLobbyMemberAttributes(const FString& Key, const FString& Value); // TODO
	void OnModifyLobbyMemberAttributesComplete(bool bWasSuccessful);
	void OnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);

	// 开始游戏
	UPROPERTY(meta = (BindWidget))
	UCommonButton* StartServerButton;
	UFUNCTION()
	void OnStartServerButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* JoinServerButton;
	UFUNCTION()
	void OnJoinServerButtonClicked();

	// 离开大厅
	UPROPERTY(meta = (BindWidget))
	UCommonButton* BackButton;
	UFUNCTION()
	void OnBackButtonClicked();
	void OnLeaveLobbyComplete(bool bWasSuccessful);
	void OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft);

	// 被踢出大厅
	void OnLobbyLeft(const FLobbyLeft& LobbyLeft);

	// 房主变更事件
	void OnPromoteLobbyMemberComplete(bool bWasSuccessful);
	void OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged);

	// 消息
	UPROPERTY(meta = (BindWidget))
	UCommonButton* SendMsgButton;
	UFUNCTION()
	void OnSendMsgButtonClicked();

};
