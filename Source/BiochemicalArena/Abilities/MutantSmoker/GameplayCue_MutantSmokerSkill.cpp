#include "GameplayCue_MutantSmokerSkill.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BiochemicalArena/Characters/MutantCharacters/MutantSmoker.h"

bool AGameplayCue_MutantSmokerSkill::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantSmoker == nullptr) MutantSmoker = Cast<AMutantSmoker>(MyTarget);
	if (MutantSmoker && MutantSmoker->SmokeEffect)
	{
		SmokeEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			MutantSmoker->SmokeEffect,
			MutantSmoker->GetActorLocation(),
			MutantSmoker->GetActorRotation()
		);
	}

	return Super::WhileActive_Implementation(MyTarget, Parameters);
}

bool AGameplayCue_MutantSmokerSkill::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantSmoker == nullptr) MutantSmoker = Cast<AMutantSmoker>(MyTarget);
	if (MutantSmoker && SmokeEffectComponent)
	{
		SmokeEffectComponent->DestroyComponent();
	}

	return Super::OnExecute_Implementation(MyTarget, Parameters);
}
