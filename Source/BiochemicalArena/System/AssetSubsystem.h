#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AssetSubsystem.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UAssetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void LoadCommonAsset();

	USoundMix* GetSoundMix();
	USoundClass* GetMasterSound();

	TArray<struct FHumanCharacterData*> GetHumanCharacterDataRows();
	TArray<struct FEquipmentData*> GetEquipmentDataRows();

	UPROPERTY()
	USoundCue* ShellSound;

	UPROPERTY()
	USoundCue* EquipSound;
	UPROPERTY()
	USoundCue* ClickSound; // 无子弹时扣动扳机的声音

	// 脚步声
	UPROPERTY()
	USoundCue* MetalSound;
	UPROPERTY()
	USoundCue* WaterSound;
	UPROPERTY()
	USoundCue* GrassSound;
	UPROPERTY()
	USoundCue* MudSound;
	UPROPERTY()
	USoundCue* CommonSound;

	// 跌落时角色叫声
	UPROPERTY()
	USoundCue* OuchSound1;
	UPROPERTY()
	USoundCue* OuchSound2;
	UPROPERTY()
	USoundCue* OuchSound3;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 系统音效
	UPROPERTY()
	USoundMix* SoundMix;
	UPROPERTY()
	USoundClass* MasterSound;
	void LoadSystemSoundAsset();

	// 数据表
	UPROPERTY()
	UDataTable* HumanCharacterDataTable;
	TArray<FHumanCharacterData*> HumanCharacterDataRows;
	UPROPERTY()
	UDataTable* EquipmentDataTable;
	TArray<FEquipmentData*> EquipmentDataRows;
	void LoadDataTableAsset();

};
