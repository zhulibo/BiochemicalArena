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

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UMenuContainer> MenuContainerClass;
	UPROPERTY()
	UMenuContainer* MenuContainer;
	void AddMenu();

};
