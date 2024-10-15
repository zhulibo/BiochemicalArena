#include "BagTab.h"

#include "BagContent.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/HorizontalBox.h"
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"

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
		
		for (int32 i = 0; i < TabContents.Num(); ++i)
		{
			if (TabContents[i] == nullptr) break;

			FString TabWidgetName = TabContents[i]->GetName();
			FName TabButtonNameID = FName(*TabWidgetName.Right(TabWidgetName.Len() - 3));
			RegisterTab(TabButtonNameID, TabButtonClass, TabContents[i], i);

			USizeBox* SizeBox = NewObject<USizeBox>(this);
			UCommonButton* TabButton = Cast<UCommonButton>(GetTabButtonBaseByID(TabButtonNameID));
			if (SizeBox && TabButton)
			{
				TabButton->ButtonText->SetText(FText::FromName(TabButtonNameID));
				SizeBox->SetWidthOverride(50);
				SizeBox->AddChild(TabButton);
				TabButtonContainer->AddChild(SizeBox);
			}
		}
	}
}
