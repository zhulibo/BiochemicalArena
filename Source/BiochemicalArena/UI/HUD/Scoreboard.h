#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Scoreboard.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UScoreboard : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class ATeamDeadMatchState* TeamDeadMatchState;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* GameTitle;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class UScoreBoardLineButton> ScoreBoardLineButtonClass;
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ScoreBoardContainer;
	UFUNCTION()
	void RefreshScoreBoard(ESlateVisibility NewVisibility);

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FPS;

};
