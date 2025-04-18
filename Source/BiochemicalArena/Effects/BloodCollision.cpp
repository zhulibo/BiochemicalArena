#include "BloodCollision.h"
#include "NiagaraSystem.h"
#include "BiochemicalArena/Characters/Data/CharacterAsset.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

void UBloodCollision::ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset)
{
	for (const FBasicParticleData& ParticleData : Data)
	{
		// TODO 只让碰撞事件触发一次
		// if (PositionIDs.Contains(ParticleData.Position)) continue;
		// PositionIDs.Add(ParticleData.Position);

		int64 TempX = FMath::RoundToInt(ParticleData.Position.X);
		if (PositionXs.Contains(TempX)) continue;
		PositionXs.Add(TempX);

		FVector WorldPos = ParticleData.Position + SimulationPositionOffset;
		FHitResult OutHit;
		FVector Start = WorldPos - FVector(0,0,1);
		FVector End   = WorldPos + FVector(0,0,1);
		GetWorld()->SweepSingleByChannel(
			OutHit,
			Start,
			End,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeSphere(1.f)
		);
		if (OutHit.bBlockingHit)
		{
			if (AssetSubsystem == nullptr) AssetSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAssetSubsystem>();
			if (AssetSubsystem && AssetSubsystem->CharacterAsset)
			{
				if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(AssetSubsystem->CharacterAsset->MI_BloodDecal, this))
				{
					FLinearColor Color = FLinearColor(ParticleData.Velocity.X, ParticleData.Velocity.Y, ParticleData.Velocity.Z, ParticleData.Size);
					MID->SetVectorParameterValue(TEXT("Color"), Color.ToFColor(true));

					auto DecalComponent = UGameplayStatics::SpawnDecalAttached(
						MID,
						FVector(7.f),
						OutHit.GetComponent(),
						NAME_None,
						OutHit.ImpactPoint,
						OutHit.ImpactNormal.Rotation(),
						EAttachLocation::KeepWorldPosition,
						5.f
					);

					DecalComponent->SetFadeOut(4.f, 1.f, false);
					DecalComponent->SetFadeScreenSize(0.004f);
				}

			}
		}
	}
}
