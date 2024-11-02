#include "DefaultConfig.h"

#include "Internationalization/Culture.h"

UDefaultConfig::UDefaultConfig()
{
	// 设置默认背包
	FBag Bag;
	Bag.Primary = DefaultPrimary;
	Bag.Secondary = DefaultSecondary;
	Bag.Melee = DefaultMelee;
	Bag.Throwing = DefaultThrowing;
	
	for (int32 i = 0; i < 4; ++i)
	{
		Bags.Add(Bag);
	}

	Language = FInternationalization::Get().GetCurrentCulture()->GetName();
}
