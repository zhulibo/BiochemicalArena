#include "GameplayCue_MutantGhostSkill.h"

#include "BiochemicalArena/Characters/MutantCharacters/MutantGhost.h"
#include "GameFramework/CharacterMovementComponent.h"

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
				DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.1f);
			}
		}
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
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MaterialInterfaces[i]);
			if (DynamicMaterial == nullptr)
			{
				DynamicMaterial = MutantGhost->GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
			}
			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.f);
			}
		}
	}

	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
