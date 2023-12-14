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
	virtual void NativeConstruct() override;

	TSharedPtr<const FLobbyMember> Member;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;

private:

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* KickButton;

};
