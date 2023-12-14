#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ABaseController : public APlayerController
{
	GENERATED_BODY()

public:
	void ShowScoreboard(bool bIsShow);
	void ShowPauseMenu();

	UPROPERTY()
	class UHUDContainer* HUDContainer;

protected:
	virtual void BeginPlay() override;

	void AddHUDContainer();

};
