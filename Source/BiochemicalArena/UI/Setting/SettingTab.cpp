#include "SettingTab.h"
#include "CommonActionWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#define LOCTEXT_NAMESPACE "BiochemicalArena"

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
		// 链接Tab和Switcher
		SetLinkedSwitcher(SettingSwitcher);
		
		for (int32 i = 0; i < TabContents.Num(); ++i)
		{
			if (TabContents[i] == nullptr) break;
			FString TabWidgetName = TabContents[i]->GetName();
			FName TabButtonNameID = FName(*TabWidgetName.Right(TabWidgetName.Len() - 3));
			RegisterTab(TabButtonNameID, TabButtonClass, TabContents[i], i);

			UCommonButton* TabButton = Cast<UCommonButton>(GetTabButtonBaseByID(TabButtonNameID));
			if (TabButton)
			{
				if (TabButtonNameID == FName(TEXT("Game")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Game", "Game"));
				}
				if (TabButtonNameID == FName(TEXT("Control")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Control", "Control"));
				}
				if (TabButtonNameID == FName(TEXT("Video")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Video", "Video"));
				}
				if (TabButtonNameID == FName(TEXT("Audio")))
				{
					TabButton->ButtonText->SetText(LOCTEXT("Audio", "Audio"));
				}
				
				TabButton->ButtonText->SetText(FText::FromName(TabButtonNameID));
				UHorizontalBoxSlot* NewSlot = Cast<UHorizontalBoxSlot>(TabButtonContainer->AddChildToHorizontalBox(TabButton));
				if (NewSlot) NewSlot->SetPadding(FMargin(10, 0, 10, 0));
			}
		}
		
		// SelectTabByID("Game");
		SelectTabByID("Control");
		// SelectTabByID("Video");
		// SelectTabByID("Audio);
	}
}

#undef LOCTEXT_NAMESPACE
