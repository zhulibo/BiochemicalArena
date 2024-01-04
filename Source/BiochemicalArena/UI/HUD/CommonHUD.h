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
	class UVerticalBox* KillLogContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UKillLogLine> KillLogLineClass;
	void AddKillLog(class ABasePlayerState* AttackerState, const FString& EquipmentName, ABasePlayerState* KilledState);

protected:
	UPROPERTY()
	ABasePlayerState* LocalPlayerState;

};
