#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "HUDContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UHUDContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	// HUD
	UPROPERTY(meta = (BindWidget))
	class UCommonHUD* CommonHUD;

	UPROPERTY(meta = (BindWidget))
	class UTeamDeadMatch* TeamDeadMatch;

	UPROPERTY(meta = (BindWidget))
	class UMutation* Mutation;
	UPROPERTY(meta = (BindWidget))
	class UMutationHuman* MutationHuman;
	UPROPERTY(meta = (BindWidget))
	class UMutationMutant* MutationMutant;

	// 计分板
	UPROPERTY(meta = (BindWidget))
	class UScoreboard* Scoreboard;

	// 轮盘选择器
	UPROPERTY(meta = (BindWidget))
	class URadialMenuContainer* RadialMenuContainer;

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* MainStack;

	void ShowPauseMenu();

	void ShowBagMenu();

	void ShowCharacterMenu();

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPauseMenu> PauseMenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBagMenu> BagMenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCharacterMenu> CharacterMenuClass;

};
