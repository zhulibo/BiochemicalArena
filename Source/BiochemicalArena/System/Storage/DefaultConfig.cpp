#include "DefaultConfig.h"

#include "Internationalization/Culture.h"

UDefaultConfig::UDefaultConfig()
{
	// 设置默认背包
	FLoadout Loadout;
	Loadout.Primary = Primary;
	Loadout.Secondary = Secondary;
	Loadout.Melee = Melee;
	Loadout.Throwing = Throwing;

	for (int32 i = 0; i < 4; ++i)
	{
		Loadouts.Add(Loadout);
	}

	Language = FInternationalization::Get().GetCurrentCulture()->GetName();
}
