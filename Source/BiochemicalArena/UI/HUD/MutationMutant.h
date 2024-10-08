#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "MutationMutant.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutationMutant : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Health;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* SkillBox;
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SkillButton;
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
