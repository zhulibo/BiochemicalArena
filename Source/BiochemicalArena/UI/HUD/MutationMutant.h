#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "MutationMutant.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutationMutant : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Health;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* SelectCharacterBox;
	UPROPERTY(meta = (BindWidget))
	class UCommonButtonBase* SelectCharacterButton;
	void ShowSelectCharacterTip(bool bIsShow);

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* SkillBox;
	UPROPERTY(meta = (BindWidget))
	UCommonButtonBase* SkillButton;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* SkillText;
	void ShowSkillUI(bool bIsShow);

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* RageText;
	void UpdateRageUI(float Rage);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	class AMutantCharacter* MutantCharacter;
	UPROPERTY()
	FGameplayTag CooldownTag;

};
