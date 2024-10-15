#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Lobby.h"
#include "PlayerLineButton.generated.h"

struct FCoolLobbyMember;

UCLASS()
class BIOCHEMICALARENA_API UPlayerLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	FCoolLobbyMember Member;

	UPROPERTY()
	class UServiceManager* ServiceManager;
	UPROPERTY()
	class UAuth* Auth;
	UPROPERTY()
	class UConnect* Connect;
	UPROPERTY()
	class ULobby* Lobby;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Status;

protected:
	virtual void NativeOnInitialized() override;

	void OnPlayerLineButtonHovered();
	void OnPlayerLineButtonUnhovered();

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* PromotePlayerButton;
	UFUNCTION()
	void OnPromotePlayerButtonClicked();
	void OnPromoteMemberComplete(bool bWasSuccessful);

	UPROPERTY(meta = (BindWidget))
	UCommonButton* KickPlayerButton;
	UFUNCTION()
	void OnKickPlayerButtonClicked();
	void OnKickMemberComplete(bool bWasSuccessful);

};
