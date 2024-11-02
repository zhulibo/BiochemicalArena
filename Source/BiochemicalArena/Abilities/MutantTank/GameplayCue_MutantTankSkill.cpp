#include "GameplayCue_MutantTankSkill.h"

#include "BiochemicalArena/Characters/MutantCharacters/MutantTank.h"

bool AGameplayCue_MutantTankSkill::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantTank == nullptr) MutantTank = Cast<AMutantTank>(MyTarget);
	if (MutantTank)
	{
		
		// 更改皮肤颜色
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MutantTank->GetMesh()->GetMaterial(0));
		if (DynamicMaterial == nullptr)
		{
			DynamicMaterial = MutantTank->GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
		}
		if (DynamicMaterial)
		{
			// TODO 有时不生效
			DynamicMaterial->SetScalarParameterValue(TEXT("SkinColorLerp"), 1.f);
		}
	}

	return Super::WhileActive_Implementation(MyTarget, Parameters);
}

bool AGameplayCue_MutantTankSkill::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantTank == nullptr) MutantTank = Cast<AMutantTank>(MyTarget);
	if (MutantTank)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MutantTank->GetMesh()->GetMaterial(0));
		if (DynamicMaterial == nullptr)
		{
			DynamicMaterial = MutantTank->GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
		}
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("SkinColorLerp"), 0.f);
		}
	}

	return Super::OnExecute_Implementation(MyTarget, Parameters);
}
