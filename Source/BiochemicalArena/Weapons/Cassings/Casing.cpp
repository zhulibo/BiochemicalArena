#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CasingMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	CasingMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);

	bFirstOnHit = true;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	const FVector RandomShell = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(GetActorForwardVector(), 10.f);
	CasingMesh->AddImpulse(RandomShell * ShellEjectionImpulsePerKg * CasingMesh->GetMass());
	SetLifeSpan(10.f);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound && bFirstOnHit)
	{
		bFirstOnHit = false;
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
}
