#include "MenuTab.h"

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
	if (MenuSwitcher && TabButtonClass && ButtonContainer)
	{
		// 链接Tab和Switcher
		SetLinkedSwitcher(MenuSwitcher);
		// 注册TabButton和TabWidget
		if (TabStorage) RegisterTab(FName("Storage"), TabButtonClass, TabStorage, 0);
		if (TabServer) RegisterTab(FName("Server"), TabButtonClass, TabServer, 1);
		if (TabShop) RegisterTab(FName("Shop"), TabButtonClass, TabShop, 2);
		// 设置tabButton的TabButtonText，并添加到ButtonContainer
		UTabButton* StorageButton = Cast<UTabButton>(GetTabButtonBaseByID("Storage"));
		if (StorageButton)
		{
			StorageButton->TabButtonText->SetText(FText::FromString("Storage"));
			ButtonContainer->AddChildToHorizontalBox(StorageButton);
		}
		UTabButton* ServerButton = Cast<UTabButton>(GetTabButtonBaseByID("Server"));
		if (ServerButton)
		{
			ServerButton->TabButtonText->SetText(FText::FromString("Server"));
			ButtonContainer->AddChildToHorizontalBox(ServerButton);
		}
		UTabButton* ShopButton = Cast<UTabButton>(GetTabButtonBaseByID("Shop"));
		if (ShopButton)
		{
			ShopButton->TabButtonText->SetText(FText::FromString("Shop"));
			ButtonContainer->AddChildToHorizontalBox(ShopButton);
		}
	}
}
