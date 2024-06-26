#include "SettingTab.h"
#include "CommonActionWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void USettingTab::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 把ActionData绑定给CommonActionWidget
	LeftTabAction->SetInputAction(PreviousTabInputActionData);
	RightTabAction->SetInputAction(NextTabInputActionData);
}

void USettingTab::NativeConstruct()
{
	Super::NativeConstruct();

	LinkSwitcher();
}

void USettingTab::LinkSwitcher()
{
	if (SettingSwitcher && TabButtonContainer && TabButtonClass)
	{
		SetLinkedSwitcher(SettingSwitcher); // 链接Tab和Switcher
		for (int32 i = 0; i < TabContent.Num(); ++i)
		{
			if (TabContent[i] == nullptr) break;
			FString TabWidgetName = TabContent[i]->GetName();
			FName TabButtonNameID = FName(*TabWidgetName.Right(TabWidgetName.Len() - 3));
			RegisterTab(TabButtonNameID, TabButtonClass, TabContent[i], i);

			UCommonButton* TabButton = Cast<UCommonButton>(GetTabButtonBaseByID(TabButtonNameID));
			if (TabButton)
			{
				TabButton->ButtonText->SetText(FText::FromName(TabButtonNameID));
				UHorizontalBoxSlot* NewSlot = Cast<UHorizontalBoxSlot>(TabButtonContainer->AddChildToHorizontalBox(TabButton));
				if (NewSlot) NewSlot->SetPadding(FMargin(10, 0, 10, 0));
			}
		}
		SelectTabByID("Control");
		// SelectTabByID("Video");
		// SelectTabByID("Audio);
	}
}
