#include "MenuTab.h"
#include "CommonActivatableWidget.h"

void UMenuTab::NativeConstruct()
{
	Super::NativeConstruct();

	// 把ActionData绑定给CommonActionWidget
	LeftTabAction->SetInputAction(PreviousTabInputActionData);
	RightTabAction->SetInputAction(NextTabInputActionData);
	LinkSwitcher();
}

void UMenuTab::LinkSwitcher()
{
	if (MenuSwitcher && TabButtonContainer && TabButtonClass)
	{
		// 链接Tab和Switcher
		SetLinkedSwitcher(MenuSwitcher);
		for (int i = 0; i < TabContent.Num(); i++)
		{
			if (TabContent[i] == nullptr) break;

			FName TabButtonNameID = FName(TabContent[i]->GetName());
			RegisterTab(TabButtonNameID, TabButtonClass, TabContent[i], i);

			UCommonButton* TabButton = Cast<UCommonButton>(GetTabButtonBaseByID(TabButtonNameID));
			if (TabButton)
			{
				TabButton->ButtonText->SetText(FText::FromName(TabButtonNameID));
				TabButton->SetPadding(FMargin(10, 0, 10, 0));
				TabButtonContainer->AddChildToHorizontalBox(TabButton);
			}
		}
		// SelectTabByID("shop");
	}
}
