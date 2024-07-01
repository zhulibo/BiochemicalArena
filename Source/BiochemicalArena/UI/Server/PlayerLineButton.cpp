#include "PlayerLineButton.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
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
	KickPlayerButton->SetIsEnabled(false); // TODO OnLobbyLeft无效，暂时禁用

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnKickLobbyMemberComplete.AddUObject(this, &ThisClass::OnKickLobbyMemberComplete);
	}
}

void UPlayerLineButton::OnPlayerLineButtonHovered()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem && EOSSubsystem->AccountInfo->AccountId == EOSSubsystem->CurrentLobby->OwnerAccountId
		&& EOSSubsystem->AccountInfo->AccountId != Member->AccountId)
	{
		PromotePlayerButton->SetVisibility(ESlateVisibility::Visible);
		KickPlayerButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlayerLineButton::OnPlayerLineButtonUnhovered()
{
	PromotePlayerButton->SetVisibility(ESlateVisibility::Hidden);
	KickPlayerButton->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerLineButton::OnPromotePlayerButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->PromoteLobbyMember(Member->AccountId);
	}
}

void UPlayerLineButton::OnKickPlayerButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->KickLobbyMember(Member->AccountId);
	}
}

void UPlayerLineButton::OnKickLobbyMemberComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Kick lobby member failed!"), false);
	}
}
