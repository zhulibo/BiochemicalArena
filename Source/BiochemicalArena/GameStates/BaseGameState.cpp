#include "BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"

void ABaseGameState::MulticastAddKillLog_Implementation(ABasePlayerState* AttackerState, const FString& WeaponName, ABasePlayerState* KilledState)
{
	ABaseController* BaseController = Cast<ABaseController>(GetWorld()->GetFirstPlayerController()); // 本地玩家控制器
	if (BaseController)
	{
		BaseController->AddKillLog(AttackerState, WeaponName, KilledState);
	}
}
