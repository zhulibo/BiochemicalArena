#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipmentAsset.generated.h"

UCLASS()
class MUTATEARENA_API UEquipmentAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 捡到装备的声音
	UPROPERTY(EditAnywhere, Category = "EquipSound")
	class UMetaSoundSource* EquipSound;
	// 无子弹时扣动扳机的声音
	UPROPERTY(EditAnywhere, Category = "ClickSound")
	UMetaSoundSource* ClickSound;

	UPROPERTY(EditAnywhere, Category = "ImpactSound")
	UMetaSoundSource* ImpactSound_Body;
	UPROPERTY(EditAnywhere, Category = "ImpactSound")
	UMetaSoundSource* ImpactSound_Concrete;
	UPROPERTY(EditAnywhere, Category = "ImpactSound")
	UMetaSoundSource* ImpactSound_Dirt;
	UPROPERTY(EditAnywhere, Category = "ImpactSound")
	UMetaSoundSource* ImpactSound_Metal;
	UPROPERTY(EditAnywhere, Category = "ImpactSound")
	UMetaSoundSource* ImpactSound_Wood;
	
};
