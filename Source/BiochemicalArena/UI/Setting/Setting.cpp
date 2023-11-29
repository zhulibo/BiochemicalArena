#include "Setting.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Menu.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void USetting::NativeConstruct()
{
	Super::NativeConstruct();

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	if (!BackButton->OnClicked().IsBoundToObject(this)) BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);
}

void USetting::OnBackButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController && MenuController->MenuContainer)
	{
		MenuController->MenuContainer->MenuStack->RemoveWidget(*MenuController->MenuContainer->MenuStack->GetActiveWidget());
	}
}
