#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameLayout.generated.h"

UENUM(BlueprintType)
enum class EHUDState : uint8
{
	Playing,
	Spectating,
	Inactive
};

enum class ECoolLoginType : uint8;

UCLASS()
class BIOCHEMICALARENA_API UGameLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// 游戏层
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* GameStack;
	// 菜单层
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* MenuStack;
	// 模态层
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* ModalStack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UHUDMutation> HUDMutationClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UHUDTeamDeadMatch> HUDTeamDeadMatchClass;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class ABaseController* BaseController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPauseMenu> PauseMenuClass;
	void ShowPauseMenu();

};
