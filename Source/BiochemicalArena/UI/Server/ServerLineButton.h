#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "ServerLineButton.generated.h"

using namespace UE::Online;

UCLASS()
class BIOCHEMICALARENA_API UServerLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	TSharedPtr<const struct FLobby> Lobby;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Server;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Mode;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Map;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Player;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Status;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ping;

};
