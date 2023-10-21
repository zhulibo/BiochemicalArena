#include "AnimNotify_Footstep.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());
	if (HumanCharacter)
	{
		HumanCharacter->PlayFootstepSound();
	}
}
