#include "ServerContainer.h"
#include "Server.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UServerContainer::NativeConstruct()
{
	Super::NativeConstruct();

	AMenuController* MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack = ServerStack; // 保存ServerStack
	}

	if (ServerClass)
	{
		UCommonActivatableWidget* Widget = ServerStack->AddWidget(ServerClass);
		if (Widget)
		{
			Server = Cast<UServer>(Widget);
		}
	}
}
