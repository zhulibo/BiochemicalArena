#include "GameplayCue_MutantRestore.h"

#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

bool AGameplayCue_MutantRestore::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (MutantCharacter == nullptr) MutantCharacter = Cast<AMutantCharacter>(MyTarget);
	if (MutantCharacter && MutantCharacter->IsLocallyControlled())
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::PlaySound, 2.f, true, 0.f);
	}

	return Super::WhileActive_Implementation(MyTarget, Parameters);
}

void AGameplayCue_MutantRestore::PlaySound()
{
	if (MutantCharacter)
	{
		UGameplayStatics::PlaySoundAtLocation(this, RestoreSound, MutantCharacter->GetActorLocation());
	}
}

bool AGameplayCue_MutantRestore::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	GetWorldTimerManager().ClearTimer(TimerHandle);

	return Super::OnExecute_Implementation(MyTarget, Parameters);
}
