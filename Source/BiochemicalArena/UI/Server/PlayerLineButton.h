#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Lobby.h"
#include "PlayerLineButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UPlayerLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:

	TSharedPtr<const FLobbyMember> Member;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UCommonButton* KickButton;

};
