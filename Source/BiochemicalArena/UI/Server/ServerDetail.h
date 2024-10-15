#pragma once

#include <eos_lobby_types.h>

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
// #include "BiochemicalArena/System/EOSSubsystem.h"
#include "ServerDetail.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UServerDetail : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class AMenuController* MenuController;
	// UPROPERTY()
	// UEOSSubsystem* EOSSubsystem;
	UPROPERTY()
	class UServiceManager* ServiceManager;
	UPROPERTY()
	class UAuth* Auth;
	UPROPERTY()
	class UConnect* Connect;
	UPROPERTY()
	class ULobby* Lobby;

	void SetLobbyUIAttr();
	void InitMapComboBox();

	void SetLobbyUIButton();
	void RefreshPlayerList();

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* Team1Container; // TODO Try to use MVVM
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* Team2Container;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerLineButton> PlayerLineButtonClass;

	void OnMemberStatusReceived(EOS_ProductUserId TargetUserId, EOS_ELobbyMemberStatus CurrentStatus);

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

	void OnUpdateLobbyComplete(bool bWasSuccessful);
	void OnLobbyUpdateReceived();

	// 成员属性
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SwitchTeamButton;
	UFUNCTION()
	void OnSwitchTeamButtonClicked();
	UPROPERTY(meta = (BindWidget))
	UCommonButton* ReadyButton;
	UFUNCTION()
	void OnReadyButtonClicked();

	// 开始游戏
	UPROPERTY(meta = (BindWidget))
	UCommonButton* StartServerButton;
	bool bIsStartingServer = false;
	UFUNCTION()
	void OnStartServerButtonClicked();
	bool CanStartServer();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* JoinServerButton;
	UFUNCTION()
	void OnJoinServerButtonClicked();

	// 离开大厅
	UPROPERTY(meta = (BindWidget))
	UCommonButton* BackButton;
	bool bIsExitingLobby = false;
	UFUNCTION()
	void OnBackButtonClicked();
	void OnLeaveLobbyComplete(bool bWasSuccessful);
	void OnDestroyLobbyComplete(bool bWasSuccessful);

	// 消息
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* MsgEditableTextBox;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* SendMsgButton;
	UFUNCTION()
	void OnSendMsgButtonClicked();

};
