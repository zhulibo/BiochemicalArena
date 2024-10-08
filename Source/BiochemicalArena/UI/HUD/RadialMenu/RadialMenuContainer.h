#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "RadialMenuContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API URadialMenuContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void ShowRadialMenu();
	void ChangeRadialMenu();
	void SelectRadialMenu(double X, double Y);
	void CloseRadialMenu();
	void SetHumanRadialMenuText();
	void SetMutantRadialMenuText();
	void OnTeamChange(ETeam TempTeam);

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* TitleEquipment;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* TitleRadio;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* TitlePaint;

	UPROPERTY(meta = (BindWidget))
	class URadialMenu* RadialMenuEquipment;
	UPROPERTY(meta = (BindWidget))
	URadialMenu* RadialMenuRadio;
	UPROPERTY(meta = (BindWidget))
	URadialMenu* RadialMenuPaint;

	int32 ActiveRadialMenuIndex = 1;
	URadialMenu* GetActiveRadialMenu();

	void SetSelectedItem();

	UPROPERTY()
	class AHumanCharacter* HumanCharacter;
	UPROPERTY()
	class AMutantCharacter* MutantCharacter;

};
