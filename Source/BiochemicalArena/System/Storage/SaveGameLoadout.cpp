#include "SaveGameLoadout.h"

#include "DefaultConfig.h"

USaveGameLoadout::USaveGameLoadout()
{
	const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>();

	if (DefaultConfig == nullptr) return;

	Bags = DefaultConfig->Bags;
	CurLoadoutIndex = DefaultConfig->CurLoadoutIndex;

	HumanCharacterName = DefaultConfig->HumanCharacterName;
	MutantCharacterName = DefaultConfig->MutantCharacterName;
}
