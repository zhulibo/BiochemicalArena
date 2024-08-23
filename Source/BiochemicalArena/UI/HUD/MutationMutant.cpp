#include "MutationMutant.h"

#include "AbilitySystemComponent.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "GameplayEffect.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "Components/HorizontalBox.h"

void UMutationMutant::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 禁用按钮，这些HUD上的按钮图标只是为了展示，实际功能由增强输入处理
	SelectCharacterButton->SetIsEnabled(false);
	SkillButton->SetIsEnabled(false);

	// TODO CD图标
	// CooldownTag = FGameplayTag::RequestGameplayTag(TAG_SKILL_CD);
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

void UMutationMutant::ShowSelectCharacterTip(bool bIsShow)
{
	SelectCharacterBox->SetVisibility(bIsShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UMutationMutant::ShowSkillUI(bool bIsShow)
{
	SkillBox->SetVisibility(bIsShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UMutationMutant::UpdateRageUI(float Rage)
{
	FNumberFormattingOptions Opts;
	Opts.RoundingMode = ERoundingMode::ToPositiveInfinity; // 向上取整
	Opts.SetUseGrouping(false); // 不使用千位分隔符

	RageText->SetText(FText::AsNumber(Rage, &Opts));
}
