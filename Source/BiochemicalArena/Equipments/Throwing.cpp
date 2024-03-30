#include "Throwing.h"

#include "EquipmentType.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Camera/CameraComponent.h"

AThrowing::AThrowing()
{
	EquipmentCate = EEquipmentCate::Throwing;
}

void AThrowing::ThrowOut()
{
	EquipmentState = EEquipmentState::Thrown;

	EquipmentMesh->SetSimulatePhysics(true);
	EquipmentMesh->SetEnableGravity(true);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	EquipmentMesh->DetachFromComponent(DetachRules);

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		UCameraComponent* CameraComponent = HumanCharacter->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			EquipmentMesh->AddImpulse(CameraComponent->GetForwardVector() * 200.f * EquipmentMesh->GetMass());
		}
	}
}

void AThrowing::ManualDestroy()
{
}
