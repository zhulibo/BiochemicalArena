#include "PickupComponent.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPickupComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

}


