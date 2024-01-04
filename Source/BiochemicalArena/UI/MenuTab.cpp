#include "MenuTab.h"
#include "CommonActionWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonTextBlock.h"
#include "Common/CommonButton.h"
#include "Components/HorizontalBox.h"

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
		SetLinkedSwitcher(MenuSwitcher); // 链接Tab和Switcher
		for (int32 i = 0; i < TabContent.Num(); ++i)
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
