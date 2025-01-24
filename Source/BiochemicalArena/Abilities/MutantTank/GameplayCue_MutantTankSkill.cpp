#include "GameplayCue_MutantTankSkill.h"

#include "BiochemicalArena/Characters/MutantCharacters/MutantTank.h"

AGameplayCue_MutantTankSkill::AGameplayCue_MutantTankSkill()
{
	bAutoDestroyOnRemove = true;
}

bool AGameplayCue_MutantTankSkill::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantTank == nullptr) MutantTank = Cast<AMutantTank>(MyTarget);
	if (MutantTank)
	{
		// 更改皮肤颜色
		MutantTank->GetMesh()->SetOverlayMaterial(MutantTank->MI_Overlay_Skill);
	}

	return Super::WhileActive_Implementation(MyTarget, Parameters);
}

bool AGameplayCue_MutantTankSkill::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantTank == nullptr) MutantTank = Cast<AMutantTank>(MyTarget);
	if (MutantTank)
	{
		if (MutantTank->GetMesh()->GetOverlayMaterial() == MutantTank->MI_Overlay_Skill)
		{
			MutantTank->GetMesh()->SetOverlayMaterial(nullptr);
		}
	}

	return Super::OnExecute_Implementation(MyTarget, Parameters);
}
