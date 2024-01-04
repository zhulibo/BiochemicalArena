#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MutationMutant.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutationMutant : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Health;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* SkillStatus;

};
