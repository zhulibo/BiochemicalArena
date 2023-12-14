#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "InfectZombie.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInfectZombie : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* SkillStatus;

};
