#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
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

	void Login();
	void Login1();
	void Login2();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UMenuContainer> MenuContainerClass;
	void AddMenuContainer();

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	void OnLoginComplete(bool bWasSuccessful);
	void OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged);


};
