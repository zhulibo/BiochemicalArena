#include "AmmoPickup.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/Equipments/Weapon.h"

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
			AWeapon* PrimaryEquipment = Combat->GetPrimaryEquipment();
			// 不判断武器是否处于满弹匣状态
			if (PrimaryEquipment)
			{
				PrimaryEquipment->SetAmmo(PrimaryEquipment->GetMagCapacity()); // TODO server
				PrimaryEquipment->SetCarriedAmmo(PrimaryEquipment->GetMaxCarriedAmmo());
				Destroy();
			}
		}
	}
}
