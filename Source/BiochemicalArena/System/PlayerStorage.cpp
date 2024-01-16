#include "PlayerStorage.h"
#include "PlayerStorageType.h"

UPlayerStorage::UPlayerStorage()
{
	// 设置默认背包
	FBag Bag;
	Bag.Primary = "AK47";
	Bag.Secondary = "Glock17";
	Bag.Melee = "Kukri";
	Bag.Throwing = "Grenade";
	for (int i = 0; i < 4; ++i)
	{
		Bags.Add(Bag);
	}
}
