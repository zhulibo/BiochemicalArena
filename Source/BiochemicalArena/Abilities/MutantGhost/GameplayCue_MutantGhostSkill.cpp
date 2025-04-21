#include "GameplayCue_MutantGhostSkill.h"

#include "BiochemicalArena/Characters/MutantCharacters/MutantGhost.h"

AGameplayCue_MutantGhostSkill::AGameplayCue_MutantGhostSkill()
{
	bAutoDestroyOnRemove = true;
}

bool AGameplayCue_MutantGhostSkill::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantGhost == nullptr) MutantGhost = Cast<AMutantGhost>(MyTarget);
	if (MutantGhost)
	{
		// 更改材质透明度
		TArray<UMaterialInterface*> MaterialInterfaces = MutantGhost->GetMesh()->GetMaterials();
		for (int32 i = 0; i < MaterialInterfaces.Num(); ++i)
		{
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MaterialInterfaces[i]);
			if (DynamicMaterial == nullptr)
			{
				DynamicMaterial = MutantGhost->GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
			}
			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.02f);
			}
		}
		
		MutantGhost->ShowOverheadWidget(false);
	}

	return Super::WhileActive_Implementation(MyTarget, Parameters);
}

bool AGameplayCue_MutantGhostSkill::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantGhost == nullptr) MutantGhost = Cast<AMutantGhost>(MyTarget);
	if (MutantGhost)
	{
		TArray<UMaterialInterface*> MaterialInterfaces = MutantGhost->GetMesh()->GetMaterials();
		for (int32 i = 0; i < MaterialInterfaces.Num(); ++i)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MaterialInterfaces[i]);
			if (MID == nullptr)
			{
				MID = MutantGhost->GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
			}
			if (MID)
			{
				MID->SetScalarParameterValue(TEXT("Opacity"), 1.f);
			}
		}
		
		MutantGhost->ShowOverheadWidget(true);
	}

	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
