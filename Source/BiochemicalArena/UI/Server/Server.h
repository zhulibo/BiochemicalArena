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
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;

	// 创建大厅
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* ServerCreateButton;
	void OnServerCreateButtonClicked();
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ULobby> LobbyClass;
	void OnCreateLobbyComplete(bool bWasSuccessful);

	// 搜索大厅
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameEditableTextBox;

	UPROPERTY(meta = (BindWidget))
	class UCommonComboBox* ModeComboBox;
	UFUNCTION()
	void OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UCommonComboBox* MapComboBox;
	UFUNCTION()
	void OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerReFreshButton;
	void OnServerReFreshButtonClicked();
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ServerLineButtonContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UServerLineButton> ServerLineButtonClass;
	void OnFindLobbyComplete(bool bWasSuccessful, const TArray<TSharedRef<const FLobby>>& Lobbies);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* ServerResetButton;
	void OnServerResetButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* PagePrevButton;
	void OnPagePrevButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* PageNextButton;
	void OnPageNextButtonClicked();

	// 加入大厅
	void OnServerLineButtonClicked(class UServerLineButton* ServerLineButton);
	void OnJoinLobbyComplete(bool bWasSuccessful);

	// 邀请
	void OnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded);
	void OnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested);

	void OnLobbyJoined(const FLobbyJoined& LobbyJoined);

};
