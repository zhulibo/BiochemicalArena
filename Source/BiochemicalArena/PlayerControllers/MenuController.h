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
	class UMenuLayout* MenuLayout;
	// ServerContainer位于Switcher中不便访问，故缓存ServerContainer中的ServerStack于此。
	UPROPERTY()
	class UCommonActivatableWidgetStack* ServerStack;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuLayout> MenuLayoutClass;
	void AddMenuLayout();

};
