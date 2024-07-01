#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
#include "Online/Lobbies.h"
#include "PlayerLineButton.generated.h"

using namespace UE::Online;

UCLASS()
class BIOCHEMICALARENA_API UPlayerLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	TSharedPtr<const FLobbyMember> Member;

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;

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

	UPROPERTY(meta = (BindWidget))
	UCommonButton* KickPlayerButton;
	UFUNCTION()
	void OnKickPlayerButtonClicked();
	void OnKickLobbyMemberComplete(bool bWasSuccessful);

};
