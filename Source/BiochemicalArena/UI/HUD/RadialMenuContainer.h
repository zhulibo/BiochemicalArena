#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
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

private:
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

	int ActiveRadialMenu = 1;
	URadialMenu* GetActiveRadialMenu();

};
