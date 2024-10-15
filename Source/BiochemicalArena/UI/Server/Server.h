#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
// #include "BiochemicalArena/System/EOSSubsystem.h"
#include "Server.generated.h"

struct FLobbiesAttr;
struct FCoolLobby;

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
	// UPROPERTY()
	// UEOSSubsystem* EOSSubsystem;
	UPROPERTY()
	class UServiceManager* ServiceManager;
	UPROPERTY()
	class ULobby* Lobby;
	UPROPERTY()
	class UP2P* P2P;

	// 创建大厅
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* CreateServerButton;
	void OnCreateServerButtonClicked();
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UServerDetail> ServerDetailClass;
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
	UCommonButton* RefreshServerButton;
	void OnRefreshServerButtonClicked();
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ServerLineButtonContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UServerLineButton> ServerLineButtonClass;
	void OnFindLobbiesComplete(bool bWasSuccessful, TArray<TSharedPtr<FCoolLobby>> FoundLobbies);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* ResetServerButton;
	void OnResetServerButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* PagePrevButton;
	void OnPagePrevButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* PageNextButton;
	void OnPageNextButtonClicked();

	// 加入大厅
	void OnServerLineButtonClicked(UServerLineButton* ServerLineButton, int32 Index);
	void OnJoinLobbyComplete(bool bWasSuccessful);

	// 邀请
	void OnLobbyInvitationAdded();
	void OnUILobbyJoinRequested();

};
