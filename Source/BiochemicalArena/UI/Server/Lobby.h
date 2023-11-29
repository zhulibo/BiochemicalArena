#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Lobby.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULobby : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	class AMenuController* MenuController;
	UFUNCTION()
	void OnBackButtonClicked();
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

	void AddPlayer();

private:
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* BackButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* SendMsgButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* JoinServerButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* StartServerButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Team1Button;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* Team2Button;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* Team1Container;
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* Team2Container;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerLine> PlayerLineButtonClass;

};
