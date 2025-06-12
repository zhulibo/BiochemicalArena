#include "MenuLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Menu.h"

void UMenuLayout::NativeConstruct()
{
	Super::NativeConstruct();

	MenuStack->AddWidget(MenuClass);
}
