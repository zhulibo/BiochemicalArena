#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Scoreboard.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UScoreboard : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void RefreshScoreBoard();

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* GameTitle;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* ScoreBoardContainer;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FPS;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class UScoreBoardLineButton> ScoreBoardLineButtonClass;

};
