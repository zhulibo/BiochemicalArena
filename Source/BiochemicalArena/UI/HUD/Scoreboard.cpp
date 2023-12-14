#include "Scoreboard.h"
#include "CommonHierarchicalScrollBox.h"
#include "ScoreBoardLineButton.h"

void UScoreboard::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshScoreBoard();
}

void UScoreboard::RefreshScoreBoard()
{
	if (ScoreBoardContainer && ScoreBoardLineButtonClass)
	{
		ScoreBoardContainer->ClearChildren();
		for (int i = 0; i < 18; ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
	}
}