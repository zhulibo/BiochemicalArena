#include "MG42Pickup.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Weapons/Weapon.h"

void AMG42Pickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
			AWeapon* MG42 = GetWorld()->SpawnActor<AWeapon>(MG42Class);
			if (MG42 == nullptr) return;
			if (MainWeapon)
			{
				// TODO server调用
				// Combat->EquipWeapon(MG42);
				// Combat->UseWeapon(MG42);
			}
			else
			{
				// Combat->EquipWeapon(MG42);
			}
			Destroy();
		}
	}
}
