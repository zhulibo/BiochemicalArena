#include "AnimNotify_FinishReloading.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"

void UAnimNotify_FinishReloading::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());
	if (HumanCharacter && HumanCharacter->GetCombat())
	{
		HumanCharacter->GetCombat()->FinishReloading();
	}
}
