#include "MenuContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Menu.h"

void UMenuContainer::NativeConstruct()
{
	Super::NativeConstruct();

	MenuStack->AddWidget(MenuClass);
}
