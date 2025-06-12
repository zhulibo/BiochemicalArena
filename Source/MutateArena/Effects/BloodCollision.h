#pragma once

#include "CoreMinimal.h"
#include "NiagaraDataInterfaceExport.h"
#include "BloodCollision.generated.h"

UCLASS()
class UBloodCollision : public UObject, public INiagaraParticleCallbackHandler
{
	GENERATED_BODY()

public:
	virtual void ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset) override;

	TSet<FVector> PositionIDs;
	TSet<int64> PositionXs;
	
	UPROPERTY()
	class UAssetSubsystem* AssetSubsystem;
	
};
