#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "EOS.h"
#include "Lobby.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULobby : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void InitPlayerList();

protected:
	UPROPERTY()
	class AMenuController* MenuController;
	UFUNCTION()
	void OnSendMsgButtonClicked();
	UFUNCTION()
	void OnJoinServerButtonClicked();
	UFUNCTION()
	void OnStartServerButtonClicked();
	UFUNCTION()
	void OnTeam1ButtonClicked();
	UFUNCTION()
	void OnTeam2ButtonClicked();
	UFUNCTION()
	void OnBackButtonClicked();

	void OnLeaveLobbyComplete(bool bWasSuccessful);
	void OnModifyLobbyAttributesComplete(bool bWasSuccessful);
	void OnModifyLobbyMemberAttributesComplete(bool bWasSuccessful);

	void OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined);
	void OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft);
	void OnLobbyLeft(const FLobbyLeft& LobbyLeft);
	void OnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged);
	void OnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);
	void OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged);

private:
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SendMsgButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* JoinServerButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* StartServerButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Team1Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Team2Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* Team1Container;
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* Team2Container;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerLineButton> PlayerLineButtonClass;

	UPROPERTY()
	UEOS* EOS;

};
