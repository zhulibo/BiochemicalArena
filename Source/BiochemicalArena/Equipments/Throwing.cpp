#include "Throwing.h"

#include "EquipmentType.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AThrowing::AThrowing()
{
	EquipmentCate = EEquipmentCate::Throwing;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(CollisionSphere);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->SetAutoActivate(false);
}

void AThrowing::ThrowOut()
{
	EquipmentState = EEquipmentState::Thrown;

	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	CollisionSphere->DetachFromComponent(DetachRules);
}

void AThrowing::ManualDestroy()
{
	// Empty function, just convenient to call when ignoring the type.
}
