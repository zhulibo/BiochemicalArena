#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Scoreboard.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UScoreboard : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class ABaseGameState* BaseGameState;

	void ShowScoreboard(bool bIsShow);

	FTimerHandle TimerHandle;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UScoreBoardLineButton> ScoreBoardLineButtonClass;
	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ScoreBoardContainer;
	UFUNCTION()
	void RefreshScoreBoard();

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* GameTitle;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FPS;

};
