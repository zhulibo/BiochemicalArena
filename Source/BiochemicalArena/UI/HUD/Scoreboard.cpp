#include "Scoreboard.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ScoreBoardLineButton.h"
#include "BiochemicalArena/GameStates/TeamDeadMatchState.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

void UScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnVisibilityChanged.AddUniqueDynamic(this, &ThisClass::RefreshScoreBoard);
}

void UScoreboard::RefreshScoreBoard(ESlateVisibility NewVisibility)
{
	if (NewVisibility != ESlateVisibility::Visible) return;

	if (TeamDeadMatchState == nullptr) TeamDeadMatchState = GetWorld()->GetGameState<ATeamDeadMatchState>();
	if (ScoreBoardContainer && TeamDeadMatchState && ScoreBoardLineButtonClass)
	{
		ScoreBoardContainer->ClearChildren();
		for (int32 i = 0; i < TeamDeadMatchState->GetTeam(ETeam::Team1).Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				ScoreBoardLineButton->Player->SetText(FText::FromString(TeamDeadMatchState->GetTeam(ETeam::Team1)[i]->GetPlayerName()));
				ScoreBoardLineButton->Score->SetText(FText::FromString(FString::FromInt(TeamDeadMatchState->GetTeam(ETeam::Team1)[i]->GetScore())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
		for (int32 i = 0; i < TeamDeadMatchState->GetTeam(ETeam::Team2).Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				ScoreBoardLineButton->Player->SetText(FText::FromString(TeamDeadMatchState->GetTeam(ETeam::Team2)[i]->GetPlayerName()));
				ScoreBoardLineButton->Score->SetText(FText::FromString(FString::FromInt(TeamDeadMatchState->GetTeam(ETeam::Team2)[i]->GetScore())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
	}
}
