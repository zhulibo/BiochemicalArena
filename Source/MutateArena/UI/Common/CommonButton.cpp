#include "CommonButton.h"

#include "CommonActionWidget.h"
#include "CommonTextBlock.h"

void UCommonButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (InputActionWidget)
	{
		ButtonText->SetText(InputActionWidget->GetDisplayText());
	}
}
