#include "AN_ShellReload.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"

void UAN_ShellReload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());
	if (HumanCharacter && HumanCharacter->GetCombatComponent())
	{
		// TODO 在服务端播放非local角色的动画蒙太奇时，动画通知会触发两次，关闭多线程动画更新会正常触发一次，待查找原因。
		// HACK 服务端同一角色短时间触发两次，直接退出
		if (HumanCharacter->HasAuthority())
		{
			if (LastUniqueID == HumanCharacter->GetUniqueID() && FPlatformTime::Seconds() - LastTriggerTime < 0.02)
			{
				return;
			}

			LastUniqueID = HumanCharacter->GetUniqueID();
			LastTriggerTime = FPlatformTime::Seconds();
		}

		UE_LOG(LogTemp, Warning, TEXT("--------------------- %d"), HumanCharacter->GetLocalRole());

		HumanCharacter->GetCombatComponent()->ShellReload();
	}
}
