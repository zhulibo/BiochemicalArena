#include "Scoreboard.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ScoreBoardLineButton.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"

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
	if (bIsShow)
	{
		// 手柄Action设为了长按/键鼠为默认，且ETriggerEvent::Triggered时，会频繁触发
		if (GetVisibility() == ESlateVisibility::Visible) return;

		SetVisibility(ESlateVisibility::Visible);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::RefreshScoreBoard, 0.5f, true, 0.f);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

// TODO 不同模式计分板
void UScoreboard::RefreshScoreBoard()
{
	if (GetVisibility() != ESlateVisibility::Visible || GetWorld()->bIsTearingDown) return;

	if (BaseGameState == nullptr) BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
	if (BaseGameState)
	{
		ScoreBoardContainer->ClearChildren();

		for (int32 i = 0; i < BaseGameState->GetTeam(ETeam::Team1).Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				FString PlayerName = BaseGameState->GetTeam(ETeam::Team1)[i]->GetPlayerName();
				ScoreBoardLineButton->Player->SetText(FText::FromString(ULibraryCommon::ObfuscatePlayerName(PlayerName, this)));
				ScoreBoardLineButton->Damage->SetText(FText::FromString(FString::FromInt(BaseGameState->GetTeam(ETeam::Team1)[i]->GetDamage())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
		for (int32 i = 0; i < BaseGameState->GetTeam(ETeam::Team2).Num(); ++i)
		{
			UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass);
			if (ScoreBoardLineButton)
			{
				FString PlayerName = BaseGameState->GetTeam(ETeam::Team2)[i]->GetPlayerName();
				ScoreBoardLineButton->Player->SetText(FText::FromString(ULibraryCommon::ObfuscatePlayerName(PlayerName, this)));
				ScoreBoardLineButton->Damage->SetText(FText::FromString(FString::FromInt(BaseGameState->GetTeam(ETeam::Team2)[i]->GetDamage())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
			}
		}
	}
}
