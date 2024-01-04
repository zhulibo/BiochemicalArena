#include "MG42Pickup.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/Equipments/Equipment.h"

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
			AEquipment* MG42 = GetWorld()->SpawnActor<AEquipment>(MG42Class); // TODO server
			if (MG42 == nullptr) return;

			if (Combat->GetCurrentEquipment() && Combat->GetCurrentEquipment()->GetEquipmentType() == EEquipmentType::Primary)
			{
				// Combat->EquipEquipment(MG42);
				// Combat->UseEquipment(MG42);
			}
			else
			{
				// Combat->EquipEquipment(MG42);
			}

			Destroy();
		}
	}
}
