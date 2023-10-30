#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIOCHEMICALARENA_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPickupComponent();
	friend class AHumanCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AHumanCharacter* Character;

public:	

};
