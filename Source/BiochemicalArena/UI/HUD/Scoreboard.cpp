#include "Scoreboard.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ScoreBoardLineButton.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"

void UScoreboard::NativeConstruct()
{
	Super::NativeConstruct();

	if (!OnVisibilityChanged.IsBound()) OnVisibilityChanged.AddDynamic(this, &ThisClass::RefreshScoreBoard);
}

void UScoreboard::RefreshScoreBoard(ESlateVisibility NewVisibility)
{
	if (NewVisibility != ESlateVisibility::Visible) return;

	if (TeamDeadMatchState == nullptr) TeamDeadMatchState = GetWorld()->GetGameState<ATeamDeadMatchState>();
	if (ScoreBoardContainer && TeamDeadMatchState && ScoreBoardLineButtonClass)
	{
		ScoreBoardContainer->ClearChildren();
		for (int i = 0; i < TeamDeadMatchState->Team1.Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				ScoreBoardLineButton->Player->SetText(FText::FromString(TeamDeadMatchState->Team1[i]->GetPlayerName()));
				ScoreBoardLineButton->Kill->SetText(FText::FromString(FString::FromInt(TeamDeadMatchState->Team1[i]->GetScore())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
		for (int i = 0; i < TeamDeadMatchState->Team2.Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				ScoreBoardLineButton->Player->SetText(FText::FromString(TeamDeadMatchState->Team2[i]->GetPlayerName()));
				ScoreBoardLineButton->Kill->SetText(FText::FromString(FString::FromInt(TeamDeadMatchState->Team2[i]->GetScore())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
	}
}
