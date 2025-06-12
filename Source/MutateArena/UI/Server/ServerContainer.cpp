#include "ServerContainer.h"
#include "Server.h"
#include "MutateArena/PlayerControllers/MenuController.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UServerContainer::NativeConstruct()
{
	Super::NativeConstruct();

	AMenuController* MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack = ServerStack;
	}

	if (ServerClass)
	{
		ServerStack->AddWidget(ServerClass);
	}
}
