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
	void InitPlayerList();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class AMenuController* MenuController;
	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SendMsgButton;
	UFUNCTION()
	void OnSendMsgButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* StartServerButton;
	UFUNCTION()
	void OnStartServerButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* JoinServerButton;
	UFUNCTION()
	void OnJoinServerButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* Team1Button;
	UFUNCTION()
	void OnTeam1ButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* Team2Button;
	UFUNCTION()
	void OnTeam2ButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* Team1Container;
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* Team2Container;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerLineButton> PlayerLineButtonClass;
	void OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined);
	void OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft);
	void OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged);

	// TODO 修改大厅属性事件
	void OnModifyLobbyAttributesComplete(bool bWasSuccessful);
	void OnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged);
	// TODO 修改大厅成员属性事件
	void OnModifyLobbyMemberAttributesComplete(bool bWasSuccessful);
	void OnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);

	void OnLobbyLeft(const FLobbyLeft& LobbyLeft);
	UPROPERTY(meta = (BindWidget))
	UCommonButton* BackButton;
	UFUNCTION()
	void OnBackButtonClicked();
	void OnLeaveLobbyComplete(bool bWasSuccessful);

};
