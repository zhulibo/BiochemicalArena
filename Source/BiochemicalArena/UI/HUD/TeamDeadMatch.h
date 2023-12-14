#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TeamDeadMatch.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTeamDeadMatch : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MatchCountdownText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Team1ScoreText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Team2ScoreText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* WeaponAmmoText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CarriedAmmoText;

};
