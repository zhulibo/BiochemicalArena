#include "BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"

void ABaseGameState::MulticastAddKillLog_Implementation(ABasePlayerState* AttackerState, const FString& EquipmentName, ABasePlayerState* KilledState)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetWorld()->GetFirstPlayerController()); // 获取本地玩家控制器
	if (BaseController)
	{
		BaseController->AddKillLog(AttackerState, EquipmentName, KilledState);
	}
}
