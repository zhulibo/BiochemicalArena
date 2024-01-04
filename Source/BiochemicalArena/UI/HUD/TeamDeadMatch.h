#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "TeamDeadMatch.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTeamDeadMatch : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MatchCountdown;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Team1Score;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Team2Score;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Health;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ammo;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CarriedAmmo;

};
