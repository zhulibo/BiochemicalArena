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
		HumanCharacter->GetCombatComponent()->ShellReload();
	}
}
