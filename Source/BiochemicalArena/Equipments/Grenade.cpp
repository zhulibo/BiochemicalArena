#include "Grenade.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AGrenade::AGrenade()
{
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->Friction = 0.6f;
}

void AGrenade::ThrowOut()
{
	Super::ThrowOut();

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		UCameraComponent* CameraComponent = HumanCharacter->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			FVector ThrowVector = CameraComponent->GetForwardVector();
			ThrowVector.Z += 0.1;
			ProjectileMovement->Velocity = ThrowVector * 2000.f;
		}
	}

	ProjectileMovement->Activate();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::ExplodeDamage, 3.f);
}

void AGrenade::ExplodeDamage()
{
	if (HumanController == nullptr)
	{
		if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
		if (HumanCharacter) HumanController = Cast<AHumanController>(HumanCharacter->GetController());
	}
	if (HumanController && HasAuthority())
	{
		TArray<AActor*> IgnoreActors;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ABaseController* BaseController = Cast<ABaseController>(*It);
			if (BaseController)
			{
				ABasePlayerState* BasePlayerState = BaseController->GetPlayerState<ABasePlayerState>();
				if (BasePlayerState && BasePlayerState->GetTeam() == OwnerTeam)
				{
					ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(BaseController->GetPawn());
					if (BaseCharacter)
					{
						IgnoreActors.Add(BaseCharacter);
					}
				}
			}
		}

		UGameplayStatics::ApplyRadialDamageWithFalloff(
			this, // World context object
			Damage, // BaseDamage
			10.f, // MinimumDamage
			GetActorLocation(), // Origin
			DamageInnerRadius, // DamageInnerRadius
			DamageOuterRadius, // DamageOuterRadius
			1.f, // DamageFalloff
			UDamageType::StaticClass(), // DamageTypeClass
			IgnoreActors, // IgnoreActors
			this, // DamageCauser
			HumanController // InstigatorController
		);
	}

	Destroy();
}

// Called when character is dead
void AGrenade::ManualDestroy()
{
	bManualDestroyed = true;

	Destroy();
}

void AGrenade::Destroyed()
{
	if (!bManualDestroyed)
	{
		if (ExplodeParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticle, GetActorTransform());
		}
		if (ExplodeSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
		}
	}

	Super::Destroyed();
}
