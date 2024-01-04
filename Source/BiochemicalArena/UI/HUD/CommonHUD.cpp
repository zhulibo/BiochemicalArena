#include "CommonHUD.h"
#include "CommonTextBlock.h"
#include "KillLogLine.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "Components/VerticalBox.h"

void UCommonHUD::AddKillLog(ABasePlayerState* AttackerState, const FString& EquipmentName, ABasePlayerState* KilledState)
{
	if (KillLogContainer && KillLogLineClass)
	{
		UKillLogLine* KillLogLine = CreateWidget<UKillLogLine>(this, KillLogLineClass);
		if (KillLogLine)
		{
			KillLogContainer->AddChild(KillLogLine);

			if (LocalPlayerState == nullptr) LocalPlayerState = GetOwningPlayerState<ABasePlayerState>();
			if (AttackerState)
			{
				KillLogLine->AttackerPlayer->SetText(FText::FromString(AttackerState->GetPlayerName()));
				if (LocalPlayerState)
				{
					if (LocalPlayerState->GetTeam() == AttackerState->GetTeam())
					{
						KillLogLine->AttackerPlayer->SetColorAndOpacity(FLinearColor::Green);
					}
					else
					{
						KillLogLine->AttackerPlayer->SetColorAndOpacity(FLinearColor::Red);
					}
				}
			}
			KillLogLine->EquipmentName->SetText(FText::FromString(EquipmentName));
			if (KilledState)
			{
				KillLogLine->KilledPlayer->SetText(FText::FromString(KilledState->GetPlayerName()));
				if (LocalPlayerState)
				{
					if (LocalPlayerState->GetTeam() == KilledState->GetTeam())
					{
						KillLogLine->KilledPlayer->SetColorAndOpacity(FLinearColor::Green);
					}
					else
					{
						KillLogLine->KilledPlayer->SetColorAndOpacity(FLinearColor::Red);
					}
				}
			}

			if (KillLogContainer->GetChildrenCount() > 5)
			{
				KillLogContainer->RemoveChildAt(0);
			}
		}
	}
}
