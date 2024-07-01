#include "PickupAmmo.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/Equipments/Weapon.h"

void APickupAmmo::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(OtherActor);
	if (HumanCharacter && HumanCharacter->GetCombatComponent())
	{
		if (AWeapon* PrimaryEquipment = HumanCharacter->GetCombatComponent()->GetPrimaryEquipment())
		{
			PrimaryEquipment->MulticastFullAmmo();

			Destroy();
		}
	}
}
