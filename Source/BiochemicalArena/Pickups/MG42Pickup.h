#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "MG42Pickup.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMG42Pickup : public APickup
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	TSubclassOf<class AWeapon> MG42Class;

};
