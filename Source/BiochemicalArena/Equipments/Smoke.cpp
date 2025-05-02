#include "Smoke.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MetaSoundSource.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "Components/SphereComponent.h"

ASmoke::ASmoke()
{
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->Friction = 0.6f;
}

void ASmoke::ThrowOut()
{
	Super::ThrowOut();

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		if (UCameraComponent* CameraComponent = HumanCharacter->FindComponentByClass<UCameraComponent>())
		{
			FVector ThrowVector = CameraComponent->GetForwardVector();
			ThrowVector.Z += 0.1;
			ProjectileMovement->Velocity = ThrowVector * 1500.f;
			ProjectileMovement->Activate();
		}
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::Explode, 2.f);
}

void ASmoke::Explode()
{
	// 不能直接销毁，同步到模拟端无法执行
	SetLifeSpan(1.f);

	float Time = 15.f;
	auto ExplodeEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		ExplodeEffect,
		GetActorLocation(),
		GetActorRotation()
	);
	if (ExplodeEffectComponent)
	{
		ExplodeEffectComponent->SetVariableFloat(TEXT("Time"), Time);
	}

	// 创建一个Actor，阻挡ECC_Visibility
	if (AActor* SmokeActor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), GetActorLocation(), GetActorRotation()))
	{
		SmokeActor->SetLifeSpan(Time);
		if (USphereComponent* SphereComponent = NewObject<USphereComponent>(SmokeActor, TEXT("SphereComponent")))
		{
			SphereComponent->RegisterComponent();
			SmokeActor->SetRootComponent(SphereComponent);
			SphereComponent->SetSphereRadius(400.f);
			SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

			// SphereComponent根组件影响力SmokeActor位置，重新设置下
			SmokeActor->SetActorLocation(GetActorLocation());
			SmokeActor->Tags.Add(TAG_SMOKE_ACTOR);
		}
	}

	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}
}
