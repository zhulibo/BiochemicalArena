#include "BAGameplayCueManager.h"

#include "AbilitySystemGlobals.h"

UBAGameplayCueManager* UBAGameplayCueManager::Get()
{
	return Cast<UBAGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

// https://github.com/tranek/GASDocumentation?tab=readme-ov-file#concepts-gc-manager
bool UBAGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	return false;
}
