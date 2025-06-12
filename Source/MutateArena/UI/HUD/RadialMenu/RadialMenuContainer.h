#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MutateArena/GameStates/BaseGameState.h"
#include "RadialMenuContainer.generated.h"

UCLASS()
class MUTATEARENA_API URadialMenuContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

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

	void ShowRadialMenu(bool bIsShow);

	void ShowRadialMenuInternal();
	void ChangeRadialMenu();
	void SelectRadialMenu(double X, double Y);
	void CloseRadialMenuInternal();
	
	void OnTeamChange(ETeam TempTeam);
	void SetHumanRadialMenuText();
	void SetMutantRadialMenuText();

	int32 ActiveRadialMenuIndex = 1;
	URadialMenu* GetActiveRadialMenu();

	void SetSelectedItem();

	UPROPERTY()
	class AHumanCharacter* HumanCharacter;
	UPROPERTY()
	class AMutantCharacter* MutantCharacter;

};
