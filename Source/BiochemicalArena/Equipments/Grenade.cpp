#include "Grenade.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MetaSoundSource.h"
#include "NiagaraFunctionLibrary.h"

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
		if (UCameraComponent* CameraComponent = HumanCharacter->FindComponentByClass<UCameraComponent>())
		{
			FVector ThrowVector = CameraComponent->GetForwardVector();
			ThrowVector.Z += 0.1;
			ProjectileMovement->Velocity = ThrowVector * 2000.f;
			ProjectileMovement->Activate();
		}
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::ExplodeDamage, 3.f);
}

void AGrenade::ExplodeDamage()
{
	if (BaseController == nullptr)
	{
		if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
		if (HumanCharacter) BaseController = Cast<ABaseController>(HumanCharacter->GetController());
	}
	if (BaseController && HasAuthority())
	{
		TArray<AActor*> IgnoreActors;
		if (OwnerTeam == ETeam::NoTeam) SetOwnerTeam();
		ETeam IgnoreTeam = OwnerTeam == ETeam::Team1 ? ETeam::Team2 : ETeam::Team1;
		if (ABaseGameState* BaseGameState = GetWorld()->GetGameState<ABaseGameState>())
		{
			for (int32 i = 0; i < BaseGameState->GetPlayerStates(IgnoreTeam).Num(); ++i)
			{
				if (BaseGameState->GetPlayerStates(IgnoreTeam)[i])
				{
					IgnoreActors.AddUnique(BaseGameState->GetPlayerStates(IgnoreTeam)[i]->GetPawn());
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
			BaseController // InstigatorController
		);
	}

	ExplodeEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		ExplodeEffect,
		GetActorLocation(),
		GetActorRotation()
	);

	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}

	Destroy();
}
