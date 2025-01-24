#include "NotifyLayout.h"

#include "CommonTextBlock.h"
#include "NotifyLineButton.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UNotifyLayout::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNotifyLayout::AddNotify(const FColor DisplayColor, const FText& Msg)
{
	if (GetWorld()->bIsTearingDown) return;

	UNotifyLineButton* NotifyLineButton = CreateWidget<UNotifyLineButton>(this, NotifyLineButtonClass);
	if (NotifyLineButton == nullptr) return;

	UVerticalBoxSlot* NewSlot = Cast<UVerticalBoxSlot>(NotifyContainer->AddChild(NotifyLineButton));
	if (NewSlot) NewSlot->SetPadding(FMargin(0, 5, 0, 5));

	NotifyLineButton->Notify->SetText(Msg);
	NotifyLineButton->Notify->SetColorAndOpacity(DisplayColor);

	if (NotifyContainer->GetChildrenCount() > 6)
	{
		NotifyContainer->RemoveChildAt(0);
	}
}
