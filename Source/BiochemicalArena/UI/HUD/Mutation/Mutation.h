#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "Mutation.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutation : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MatchCountdown;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* HumanNumber;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* MutantNumber;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CurRound;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* TotalRound;

protected:
	virtual void NativeOnInitialized() override;

	void OnMatchCountdownChange(int32 CountdownTime);
	void OnTeamNumChange(int32 TeamNum, ETeam Team);
	void OnCurRoundChange(int32 Round);
	void OnTotalRoundChange(int32 Round);

};
