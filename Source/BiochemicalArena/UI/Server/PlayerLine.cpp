#include "PlayerLine.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"

void UPlayerLine::NativeConstruct()
{
	Super::NativeConstruct();

	KickButton->ButtonText->SetText(FText::FromString("Kick"));
}
