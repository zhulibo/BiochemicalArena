#include "HUDMutation.h"

#include "MutationHuman.h"
#include "MutationMutant.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Components/DynamicEntryBox.h"

void UHUDMutation::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// HACK 提前实例化UMutationHuman/UMutationMutant，避免委托触发时还未绑定
	MutationHuman = Cast<UMutationHuman>(EntryBox_MutationHuman->CreateEntry());
	MutationMutant = Cast<UMutationMutant>(EntryBox_MutationMutant->CreateEntry());
	EntryBox_MutationHuman->Reset();
	EntryBox_MutationMutant->Reset();

	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->OnTeamChange.AddUObject(this, &ThisClass::OnTeamChange);
	}
}

void UHUDMutation::OnTeamChange(ETeam Team)
{
	if (Team == ETeam::Team1)
	{
		EntryBox_MutationMutant->Reset();
		MutationHuman = Cast<UMutationHuman>(EntryBox_MutationHuman->CreateEntry());
	}
	else if (Team == ETeam::Team2)
	{
		EntryBox_MutationHuman->Reset();
		MutationMutant = Cast<UMutationMutant>(EntryBox_MutationMutant->CreateEntry());
	}
}
