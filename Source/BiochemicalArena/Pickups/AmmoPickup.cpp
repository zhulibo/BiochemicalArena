#include "AmmoPickup.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Weapons/Weapon.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(OtherActor);
	if (HumanCharacter)
	{
		UCombatComponent* Combat = HumanCharacter->GetCombat();
		if (Combat)
		{
			AWeapon* MainWeapon = Combat->GetMainWeapon();
			/**
			 * 不判断武器是否处于满弹匣状态，避免拾取不成功时
			 * 1 玩家猜到补给箱内物品
			 * 2 让玩家误以为是bug，再次走到补给箱上拾取
			 */
			if (MainWeapon && HasAuthority())
			{
				MainWeapon->SetAmmo(MainWeapon->GetMagCapacity());
				MainWeapon->SetCarriedAmmo(MainWeapon->GetMaxCarriedAmmo());
				Destroy();
			}
		}
	}
}
