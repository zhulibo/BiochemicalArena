#include "MenuTab.h"
#include "CommonActionWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonTextBlock.h"
#include "Common/CommonButton.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#define LOCTEXT_NAMESPACE "BiochemicalArena"

void UMenuTab::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 把ActionData绑定给CommonActionWidget
	LeftTabAction->SetInputAction(PreviousTabInputActionData);
	RightTabAction->SetInputAction(NextTabInputActionData);
}

void UMenuTab::NativeConstruct()
{
	Super::NativeConstruct();

	LinkSwitcher();
}

void UMenuTab::LinkSwitcher()
{
	if (MenuSwitcher && TabButtonContainer && TabButtonClass)
	{
		// 链接Tab和Switcher
		SetLinkedSwitcher(MenuSwitcher);
		
		for (int32 i = 0; i < TabContents.Num(); ++i)
		{
			if (TabContents[i] == nullptr) break;
			FName TabButtonNameID = FName(TabContents[i]->GetName());
			RegisterTab(TabButtonNameID, TabButtonClass, TabContents[i], i);

			UCommonButton* TabButton = Cast<UCommonButton>(GetTabButtonBaseByID(TabButtonNameID));
			if (TabButton)
			{
				if (TabButtonNameID == FName(TEXT("Server")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Server", "Server"));
				}
				if (TabButtonNameID == FName(TEXT("Storage")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Storage", "Storage"));
				}
				if (TabButtonNameID == FName(TEXT("Shop")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Shop", "Shop"));
				}
				if (TabButtonNameID == FName(TEXT("Dev")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Dev", "Dev"));
				}
				
				UHorizontalBoxSlot* NewSlot = Cast<UHorizontalBoxSlot>(TabButtonContainer->AddChildToHorizontalBox(TabButton));
				if (NewSlot) NewSlot->SetPadding(FMargin(10, 0, 10, 0));
			}
		}
		
		// SelectTabByID("shop");
	}
}

#undef LOCTEXT_NAMESPACE
