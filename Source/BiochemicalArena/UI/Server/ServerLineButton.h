#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Lobby.h"
#include "ServerLineButton.generated.h"

using namespace UE::Online;

UCLASS()
class BIOCHEMICALARENA_API UServerLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	TSharedPtr<FCoolLobby> Lobby;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Server;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Mode;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Map;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Player;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ping;

};
