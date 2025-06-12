#include "Scoreboard.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ScoreBoardLineButton.h"
#include "MutateArena/MutateArena.h"
#include "MutateArena/GameStates/BaseGameState.h"
#include "MutateArena/PlayerControllers/BaseController.h"
#include "MutateArena/PlayerStates/BasePlayerState.h"
#include "MutateArena/PlayerStates/TeamType.h"
#include "MutateArena/Utils/LibraryCommon.h"

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
		// ETriggerEvent::Triggered时调用ShowScoreboard，会频繁触发
		if (GetVisibility() == ESlateVisibility::Visible) return;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::RefreshScoreBoard, 0.5f, true, 0.f);

		SetVisibility(ESlateVisibility::Visible);
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
	if (GetVisibility() != ESlateVisibility::Visible) return;

	if (BaseGameState == nullptr) BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
	if (BaseGameState)
	{
		ScoreBoardContainer->ClearChildren();
		
		TArray<ABasePlayerState*> PlayerStates;
		PlayerStates.Append(BaseGameState->GetPlayerStates({}));

		// 删除空对象
		PlayerStates.RemoveAll([](const ABasePlayerState* BasePlayerState) {
			if (BasePlayerState == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Found a null pointer in PlayerStates, removing it."));
			}
			return BasePlayerState == nullptr;
		});

		// 按照伤害排序
		Algo::Sort(PlayerStates, [](const ABasePlayerState* A, const ABasePlayerState* B) {
			return A->GetDamage() > B->GetDamage();
		});

		for (int32 i = 0; i < PlayerStates.Num(); ++i)
		{
			if (UScoreBoardLineButton* ScoreBoardLineButton = CreateWidget<UScoreBoardLineButton>(this, ScoreBoardLineButtonClass))
			{
				FString PlayerName = PlayerStates[i]->GetPlayerName();
				ScoreBoardLineButton->Player->SetText(FText::FromString(ULibraryCommon::ObfuscatePlayerName(PlayerName, this)));
				ScoreBoardLineButton->Damage->SetText(FText::FromString(FString::FromInt(PlayerStates[i]->GetDamage())));
				ScoreBoardContainer->AddChild(ScoreBoardLineButton);
				if (PlayerStates[i]->GetTeam() == ETeam::Team2)
				{
					ScoreBoardLineButton->SetColorAndOpacity(C_GREEN);
				}
			}
		}
	}
}
