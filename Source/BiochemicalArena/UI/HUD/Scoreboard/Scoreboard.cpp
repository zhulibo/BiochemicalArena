#include "Scoreboard.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ScoreBoardLineButton.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

void UScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ABaseController* BaseController = Cast<ABaseController>(GetOwningPlayer()))
	{
		BaseController->ShowScoreboard.AddUObject(this, &ThisClass::ShowScoreboard);
	}
}

void UScoreboard::ShowScoreboard(bool bIsShow)
{
	SetVisibility(bIsShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if (bIsShow)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::RefreshScoreBoard, 0.5f, true, 0.f);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

void UScoreboard::RefreshScoreBoard()
{
	if (GetVisibility() != ESlateVisibility::Visible) return;

	if (BaseGameState == nullptr) BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
	if (ScoreBoardContainer && BaseGameState && ScoreBoardLineButtonClass)
	{
		ScoreBoardContainer->ClearChildren();
		for (int32 i = 0; i < BaseGameState->GetTeam(ETeam::Team1).Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				ScoreBoardLineButton->Player->SetText(FText::FromString(BaseGameState->GetTeam(ETeam::Team1)[i]->GetPlayerName()));
				ScoreBoardLineButton->Damage->SetText(FText::FromString(FString::FromInt(BaseGameState->GetTeam(ETeam::Team1)[i]->GetDamage())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
		for (int32 i = 0; i < BaseGameState->GetTeam(ETeam::Team2).Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				ScoreBoardLineButton->Player->SetText(FText::FromString(BaseGameState->GetTeam(ETeam::Team2)[i]->GetPlayerName()));
				ScoreBoardLineButton->Damage->SetText(FText::FromString(FString::FromInt(BaseGameState->GetTeam(ETeam::Team2)[i]->GetDamage())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
	}
}
