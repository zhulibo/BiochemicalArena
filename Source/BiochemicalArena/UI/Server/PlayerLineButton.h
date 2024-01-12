#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Online/Lobbies.h"
#include "PlayerLineButton.generated.h"

using namespace UE::Online;

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
	class UCommonButton* KickButton;

};
