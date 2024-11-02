#include "MutationMutant.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "GameplayEffect.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/HorizontalBox.h"

void UMutationMutant::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->OnMutantHealthChange.AddUObject(this, &ThisClass::OnMutantHealthChange);
		MutationController->OnSkillChange.AddUObject(this, &ThisClass::OnSkillChange);
		MutationController->OnRageChange.AddUObject(this, &ThisClass::OnRageChange);
	}

	SkillButton->OnClicked().AddUObject(this, &ThisClass::OnSkillButtonClicked);

	// CooldownTag = FGameplayTag::RequestGameplayTag(TAG_Mutant_SKILL_CD);
}

void UMutationMutant::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// TODO CD图标
	// if (MutantCharacter == nullptr) MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn());
	// if (MutantCharacter && MutantCharacter->GetAbilitySystemComponent())
	// {
	// 	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
	// 	TArray<float> Times = MutantCharacter->GetAbilitySystemComponent()->GetActiveEffectsTimeRemaining(Query);
	// }
}

void UMutationMutant::OnMutantHealthChange(float TempHealth)
{
	FNumberFormattingOptions Opts;
	Opts.RoundingMode = ERoundingMode::ToPositiveInfinity; // 向上取整
	Opts.SetUseGrouping(false); // 不使用千位分隔符
	Health->SetText(FText::AsNumber(TempHealth, &Opts));
}

void UMutationMutant::OnSkillChange(bool bIsShow)
{
	SkillBox->SetVisibility(bIsShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

// TODO 验证通过UI接受事件可行性，将CommonUI与增强输入结合使用。
void UMutationMutant::OnSkillButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSkillButtonClicked"));
}

void UMutationMutant::OnRageChange(float TempRage)
{
	FNumberFormattingOptions Opts;
	Opts.RoundingMode = ERoundingMode::ToPositiveInfinity; // 向上取整
	Opts.SetUseGrouping(false); // 不使用千位分隔符

	Rage->SetText(FText::AsNumber(TempRage, &Opts));
}
