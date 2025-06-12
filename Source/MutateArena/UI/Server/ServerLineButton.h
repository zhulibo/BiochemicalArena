#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "MutateArena/System/EOSSubsystem.h"
#include "ServerLineButton.generated.h"

UCLASS()
class MUTATEARENA_API UServerLineButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	TSharedPtr<const FLobby> Lobby;

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
