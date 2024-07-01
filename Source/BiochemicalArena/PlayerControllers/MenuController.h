#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class UMenuContainer* MenuContainer;
	UPROPERTY()
	class UCommonActivatableWidgetStack* ServerStack;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuContainer> MenuContainerClass;
	void AddMenuContainer();

};
