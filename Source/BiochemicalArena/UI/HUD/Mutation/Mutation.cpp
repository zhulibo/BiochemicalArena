#include "Mutation.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"

void UMutation::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->OnMatchCountdownChange.AddUObject(this, &ThisClass::OnMatchCountdownChange);
		MutationController->OnTeamNumChange.AddUObject(this, &ThisClass::OnTeamNumChange);
		MutationController->OnCurRoundChange.AddUObject(this, &ThisClass::OnCurRoundChange);
		MutationController->OnTotalRoundChange.AddUObject(this, &ThisClass::OnTotalRoundChange);
	}
}

void UMutation::OnMatchCountdownChange(int32 CountdownTime)
{
	FString CountdownText = ULibraryCommon::GetFormatTime(CountdownTime);
	MatchCountdown->SetText(FText::FromString(CountdownText));
}

void UMutation::OnTeamNumChange(int32 TeamNum, ETeam Team)
{
	switch (Team)
	{
	case ETeam::Team1:
		HumanNumber->SetText(FText::AsNumber(TeamNum));
		break;
	case ETeam::Team2:
		MutantNumber->SetText(FText::AsNumber(TeamNum));
		break;
	}
}

void UMutation::OnCurRoundChange(int32 Round)
{
	CurRound->SetText(FText::AsNumber(Round));
}

void UMutation::OnTotalRoundChange(int32 Round)
{
	TotalRound->SetText(FText::AsNumber(Round));
}
