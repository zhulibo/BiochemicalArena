#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HumanController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	class AHumanHUD* HumanHUD;

};
