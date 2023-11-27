#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class UMenuContainer* MenuContainer;

protected:
	virtual void BeginPlay() override;
	void AddMenuContainer();

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UMenuContainer> MenuContainerClass;

};
