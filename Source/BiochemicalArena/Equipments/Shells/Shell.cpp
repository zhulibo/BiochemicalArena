#include "Shell.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

AShell::AShell()
{
	PrimaryActorTick.bCanEverTick = false;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMesh"));
	SetRootComponent(ShellMesh);

	ShellMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ShellMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	ShellMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	ShellMesh->SetSimulatePhysics(true);
	ShellMesh->SetEnableGravity(true);
	ShellMesh->SetNotifyRigidBodyCollision(true);
}

void AShell::BeginPlay()
{
	Super::BeginPlay();

	ShellMesh->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnHit);

	ShellMesh->SetPhysicsLinearVelocity(InitVelocity); // 叠加角色移速
	const FVector RandomShell = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(GetActorForwardVector(), 10.f);
	ShellMesh->AddImpulse(RandomShell * 200.f, NAME_None, true);

	SetLifeSpan(10.f);
}

void AShell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsFirstOnHit) // 第一次触地时播放声音
	{
		bIsFirstOnHit = false;

		if (ShellSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		}
	}
}
