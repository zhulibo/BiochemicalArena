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
	UCommonTextBlock* KillStreak;

protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY()
	class ABasePlayerState* LocalPlayerState;
	
	void OnAnnouncementChange(FText Text);
	void OnKillStreakChange(int32 Num);

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* KillLogContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UKillLogLine> KillLogLineClass;
	void OnAddKillLog(ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState);

};
