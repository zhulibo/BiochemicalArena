#include "MutationHuman.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

void UMutationHuman::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->OnHumanHealthChange.AddUObject(this, &ThisClass::OnHumanHealthChange);
		MutationController->OnAmmoChange.AddUObject(this, &ThisClass::OnAmmoChange);
		MutationController->OnCarriedAmmoChange.AddUObject(this, &ThisClass::OnCarriedAmmoChange);
		MutationController->OnCause1000Damage.AddUObject(this, &ThisClass::OnCause1000Damage);
		MutationController->OnDamageMulChange.AddUObject(this, &ThisClass::OnDamageMulChange);
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

void UMutationHuman::OnCause1000Damage()
{
	DamageIcon->SetText(FText::FromString(TEXT("1000 DAMAGE")));
	
	GetWorld()->GetTimerManager().SetTimer(DamageIconTimerHandle, this, &ThisClass::ClearDamageIcon, 5.f);
}

void UMutationHuman::ClearDamageIcon()
{
	DamageIcon->SetText(FText::FromString(TEXT("")));
}

void UMutationHuman::OnDamageMulChange(float TempDamageMul)
{
	DamageMul->SetText(FText::AsPercent(TempDamageMul));
}
