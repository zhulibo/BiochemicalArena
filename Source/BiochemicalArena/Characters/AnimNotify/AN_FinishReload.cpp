#include "AN_FinishReload.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"

void UAN_FinishReload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());
	if (HumanCharacter && HumanCharacter->GetCombatComponent())
	{
		// TODO 在服务端播放ROLE_SimulatedProxy角色动画蒙太奇 会触发两次通知
		if (HumanCharacter->HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("Notify server"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Notify client"));
		}

		HumanCharacter->GetCombatComponent()->FinishReload();
	}
}
