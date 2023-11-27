#include "MenuContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UMenuContainer::NativeConstruct()
{
	Super::NativeConstruct();

	MenuStack->AddWidget(Menu);
}
