#include "MAGameplayCueManager.h"

#include "AbilitySystemGlobals.h"

UMAGameplayCueManager* UMAGameplayCueManager::Get()
{
	return Cast<UMAGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

// https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-gc-manager
bool UMAGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	return false;
}
