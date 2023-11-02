#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "AmmoPickup.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AAmmoPickup : public APickup
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

};
