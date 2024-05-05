#include "PlayerLineButton.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"

void UPlayerLineButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnHovered().AddUObject(this, &ThisClass::OnPlayerLineButtonHovered);
	OnUnhovered().AddUObject(this, &ThisClass::OnPlayerLineButtonUnhovered);

	PromotePlayerButton->ButtonText->SetText(FText::FromString("Promote"));
	PromotePlayerButton->OnClicked().AddUObject(this, &ThisClass::OnPromotePlayerButtonClicked);
	PromotePlayerButton->SetVisibility(ESlateVisibility::Hidden);

	KickPlayerButton->ButtonText->SetText(FText::FromString("Kick"));
	KickPlayerButton->OnClicked().AddUObject(this, &ThisClass::OnKickPlayerButtonClicked);
	KickPlayerButton->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerLineButton::OnPlayerLineButtonHovered()
{
	PromotePlayerButton->SetVisibility(ESlateVisibility::Visible);
	KickPlayerButton->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerLineButton::OnPlayerLineButtonUnhovered()
{
	PromotePlayerButton->SetVisibility(ESlateVisibility::Hidden);
	KickPlayerButton->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerLineButton::OnPromotePlayerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnPromoteServerButtonClicked"));
}

void UPlayerLineButton::OnKickPlayerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnKickServerButtonClicked"));
}
