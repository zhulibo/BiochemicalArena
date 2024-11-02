#include "Pickup.h"

#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "Components/SphereComponent.h"

APickup::APickup()
{
	bReplicates = true;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	SetRootComponent(OverlapSphere);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_TEAM1_MESH, ECollisionResponse::ECR_Overlap);
	OverlapSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &APickup::OnSphereOverlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(RootComponent);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (MutationGameState == nullptr) MutationGameState = GetWorld()->GetGameState<AMutationGameState>();
	if (MutationGameState)
	{
		MutationGameState->OnRoundStarted.AddUObject(this, &ThisClass::OnRoundStarted);
	}
}

void APickup::OnRoundStarted()
{
	Destroy();
}
