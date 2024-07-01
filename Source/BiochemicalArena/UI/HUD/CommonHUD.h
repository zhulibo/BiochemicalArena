#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommonHUD.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCommonHUD : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Announcement;

	UPROPERTY(meta = (BindWidget))
	class UCrosshair* Crosshair;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* KillLogContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UKillLogLine> KillLogLineClass;
	void AddKillLog(class ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState);

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* KillStreak;

protected:
	UPROPERTY()
	ABasePlayerState* LocalPlayerState;

};
