#include "FootstepAN.h"
#include "HumanCharacter.h"

void UFootstepAN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());
	if (HumanCharacter)
	{
		HumanCharacter->PlayFootstepSound();
	}
}
