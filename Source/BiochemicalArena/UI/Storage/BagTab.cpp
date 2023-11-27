#include "BagTab.h"

#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "CommonTextBlock.h"

void UBagTab::NativeConstruct()
{
	Super::NativeConstruct();

	LinkSwitcher();
}

void UBagTab::LinkSwitcher()
{
	if (BagSwitcher && TabButtonContainer && TabButtonClass)
	{
		// 链接Tab和Switcher
		SetLinkedSwitcher(BagSwitcher);
		for (int i = 0; i < TabContent.Num(); i++)
		{
			if (TabContent[i] == nullptr) break;

			FString TabWidgetName = TabContent[i]->GetName();
			FName TabButtonNameID = FName(*TabWidgetName.Right(TabWidgetName.Len() - 3));
			RegisterTab(TabButtonNameID, TabButtonClass, TabContent[i], i);

			USizeBox* SizeBox = NewObject<USizeBox>(this);

			UCommonButton* TabButton = Cast<UCommonButton>(GetTabButtonBaseByID(TabButtonNameID));
			if (SizeBox && TabButton)
			{
				SizeBox->SetWidthOverride(50);
				SizeBox->AddChild(TabButton);
				TabButton->ButtonText->SetText(FText::FromName(TabButtonNameID));
				if (i != TabContent.Num() -1) TabButton->SetPadding(FMargin(0, 0, 5, 0));
				TabButtonContainer->AddChild(SizeBox);
			}
		}
	}
}
