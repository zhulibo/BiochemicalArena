#include "MutationHuman.h"

#include "CommonTextBlock.h"
#include "MetaSoundSource.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/Data/CommonAsset.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void UMutationHuman::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->OnHumanHealthChange.AddUObject(this, &ThisClass::OnHumanHealthChange);
		MutationController->OnAmmoChange.AddUObject(this, &ThisClass::OnAmmoChange);
		MutationController->OnCarriedAmmoChange.AddUObject(this, &ThisClass::OnCarriedAmmoChange);
		MutationController->OnDamageMulChange.AddUObject(this, &ThisClass::OnDamageMulChange);
		MutationController->OnCause1000Damage.AddUObject(this, &ThisClass::OnCombatIconChange, ECombatIconType::Cause1000Damage);
		MutationController->OnBeImmune.AddUObject(this, &ThisClass::OnCombatIconChange, ECombatIconType::BeImmune);
	}
}

void UMutationHuman::OnHumanHealthChange(float TempHealth)
{
	FNumberFormattingOptions Opts;
	Opts.RoundingMode = ERoundingMode::ToPositiveInfinity; // 向上取整
	Opts.SetUseGrouping(false); // 不使用千位分隔符
	Health->SetText(FText::AsNumber(TempHealth, &Opts));
}

void UMutationHuman::OnAmmoChange(int32 TempAmmo)
{
	Ammo->SetText(FText::AsNumber(TempAmmo));
}

void UMutationHuman::OnCarriedAmmoChange(int32 TempCarriedAmmo)
{
	CarriedAmmo->SetText(FText::AsNumber(TempCarriedAmmo));
}

void UMutationHuman::OnDamageMulChange(float TempDamageMul)
{
	DamageMul->SetText(FText::AsPercent(TempDamageMul));
}

void UMutationHuman::OnCombatIconChange(ECombatIconType CombatIconType)
{
	UE_LOG(LogTemp, Warning, TEXT("1"));
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->CommonAsset == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("2"));
	switch (CombatIconType)
	{
	case ECombatIconType::Cause1000Damage:
		UE_LOG(LogTemp, Warning, TEXT("3"));
		CombatIcon->SetText(FText::FromString(TEXT("1000 DAMAGE")));
		
		if (UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(this, AssetSubsystem->CommonAsset->Cause1000DamageSound))
		{
			// AudioComponent->SetFloatParameter(TEXT("Index"), 1);
		}
		
		break;
		
	case ECombatIconType::BeImmune:
		CombatIcon->SetText(FText::FromString(TEXT("BeImmune")));
		
		if (UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(this, AssetSubsystem->CommonAsset->Cause1000DamageSound))
		{
			// AudioComponent->SetFloatParameter(TEXT("Index"), 1);
		}
		
		break;
	}

	GetWorld()->GetTimerManager().SetTimer(CombatIconTimerHandle, this, &ThisClass::ClearCombatIcon, 5.f);
}

void UMutationHuman::ClearCombatIcon()
{
	CombatIcon->SetText(FText::GetEmpty());
}
