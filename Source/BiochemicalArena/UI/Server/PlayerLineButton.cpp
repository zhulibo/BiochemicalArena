#include "PlayerLineButton.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"

void UPlayerLineButton::NativeConstruct()
{
	Super::NativeConstruct();

	KickButton->ButtonText->SetText(FText::FromString("Kick"));
}
