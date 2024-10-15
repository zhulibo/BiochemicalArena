#include "CommonHUD.h"
#include "CommonTextBlock.h"
#include "KillLogLine.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "Components/VerticalBox.h"

#define LOCTEXT_NAMESPACE "BiochemicalArena"

void UCommonHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ABaseController* BaseController = Cast<ABaseController>(GetOwningPlayer()))
	{
		BaseController->ChangeAnnouncement.AddUObject(this, &ThisClass::OnAnnouncementChange);
		BaseController->OnKillStreakChange.AddUObject(this, &ThisClass::OnKillStreakChange);
	}

	if (ABaseGameState* BaseGameState = GetWorld()->GetGameState<ABaseGameState>())
	{
		BaseGameState->OnAddKillLog.AddUObject(this, &ThisClass::OnAddKillLog);
	}
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
		KillStreak->SetText(FText::FromString(TEXT("")));
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
				KillLogLine->AttackerPlayer->SetText(FText::FromString(AttackerState->GetPlayerName()));
				if (LocalPlayerState)
				{
					if (LocalPlayerState->GetTeam() == AttackerState->GetTeam())
					{
						KillLogLine->AttackerPlayer->SetColorAndOpacity(C_GREEN);
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
				KillLogLine->DamagedPlayer->SetText(FText::FromString(DamagedState->GetPlayerName()));
				if (LocalPlayerState)
				{
					if (LocalPlayerState->GetTeam() == DamagedState->GetTeam())
					{
						KillLogLine->DamagedPlayer->SetColorAndOpacity(C_GREEN);
					}
					else
					{
						KillLogLine->DamagedPlayer->SetColorAndOpacity(C_RED);
					}
				}
			}

			// 限制击杀日志条数
			if (KillLogContainer->GetChildrenCount() > 5)
			{
				KillLogContainer->RemoveChildAt(0);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
