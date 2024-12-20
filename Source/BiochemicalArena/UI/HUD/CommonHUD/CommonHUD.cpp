#include "CommonHUD.h"

#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/HUD/KillLogLine.h"
#include "Spectator.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/UI/GameLayout.h"
#include "BiochemicalArena/UI/TextChat/TextChat.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"

#define LOCTEXT_NAMESPACE "UCommonHUD"

void UCommonHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ABaseController* BaseController = Cast<ABaseController>(GetOwningPlayer()))
	{
		BaseController->ChangeAnnouncement.AddUObject(this, &ThisClass::OnAnnouncementChange);
		BaseController->OnKillStreakChange.AddUObject(this, &ThisClass::OnKillStreakChange);
		BaseController->OnHUDStateChange.AddUObject(this, &ThisClass::OnHUDStateChange);
	}

	if (ABaseGameState* BaseGameState = GetWorld()->GetGameState<ABaseGameState>())
	{
		BaseGameState->OnAddKillLog.AddUObject(this, &ThisClass::OnAddKillLog);
	}
	
	// 默认隐藏聊天输入框
	TextChat->MsgEditableTextBox->SetVisibility(ESlateVisibility::Hidden);
	TextChat->MsgContainer->SetScrollBarVisibility(ESlateVisibility::Hidden);
}

void UCommonHUD::OnAnnouncementChange(FText Text)
{
	Announcement->SetText(Text);
}

void UCommonHUD::OnKillStreakChange(int Num)
{
	if (Num > 1)
	{
		KillStreak->SetText(FText::FromString(FString::Printf(TEXT("%d KILL"), Num)));
	}
	else
	{
		KillStreak->SetText(FText::GetEmpty());
	}
}

void UCommonHUD::OnAddKillLog(ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState)
{
	if (AttackerState == nullptr || DamagedState == nullptr) return;

	if (KillLogContainer && KillLogLineClass)
	{
		UKillLogLine* KillLogLine = CreateWidget<UKillLogLine>(this, KillLogLineClass);
		if (KillLogLine)
		{
			if (LocalPlayerState == nullptr) LocalPlayerState = GetOwningPlayerState<ABasePlayerState>();
			KillLogContainer->AddChild(KillLogLine);

			// 攻击者
			if (AttackerState)
			{
				
				KillLogLine->AttackerPlayer->SetText(FText::FromString(ULibraryCommon::ObfuscatePlayerName(AttackerState->GetPlayerName(), this)));
				if (LocalPlayerState)
				{
					if (LocalPlayerState->GetTeam() == AttackerState->GetTeam())
					{
						KillLogLine->AttackerPlayer->SetColorAndOpacity(C_BLUE);
					}
					else
					{
						KillLogLine->AttackerPlayer->SetColorAndOpacity(C_RED);
					}
				}
			}

			// 击杀原因
			KillLogLine->CauserName->SetText(FText::FromString(CauserName));

			// 死亡者
			if (DamagedState)
			{
				KillLogLine->DamagedPlayer->SetText(FText::FromString(ULibraryCommon::ObfuscatePlayerName(DamagedState->GetPlayerName(), this)));
				if (LocalPlayerState)
				{
					if (LocalPlayerState->GetTeam() == DamagedState->GetTeam())
					{
						KillLogLine->DamagedPlayer->SetColorAndOpacity(C_BLUE);
					}
					else
					{
						KillLogLine->DamagedPlayer->SetColorAndOpacity(C_RED);
					}
				}
			}

			// 限制击杀日志条数
			if (KillLogContainer->GetChildrenCount() > 6)
			{
				KillLogContainer->RemoveChildAt(0);
			}
		}
	}
}

void UCommonHUD::OnHUDStateChange(EHUDState HUDState)
{
	if (HUDState == EHUDState::Spectating)
	{
		Spectator->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Spectator->SetVisibility(ESlateVisibility::Collapsed);
	}
}

#undef LOCTEXT_NAMESPACE
