#include "CommonHUD.h"
#include "CommonTextBlock.h"
#include "KillLogLine.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "Components/VerticalBox.h"

void UCommonHUD::AddKillLog(ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState)
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
						KillLogLine->AttackerPlayer->SetColorAndOpacity(COLOR_MUTANT);
					}
					else
					{
						KillLogLine->AttackerPlayer->SetColorAndOpacity(COLOR_HUMAN);
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
						KillLogLine->DamagedPlayer->SetColorAndOpacity(COLOR_MUTANT);
					}
					else
					{
						KillLogLine->DamagedPlayer->SetColorAndOpacity(COLOR_HUMAN);
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
