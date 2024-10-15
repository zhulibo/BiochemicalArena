#include "PlayerLineButton.h"

#include "CommonTextBlock.h"
#include "Connect.h"
#include "Lobby.h"
#include "ServiceManager.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"

void UPlayerLineButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnHovered().AddUObject(this, &ThisClass::OnPlayerLineButtonHovered);
	OnUnhovered().AddUObject(this, &ThisClass::OnPlayerLineButtonUnhovered);

	PromotePlayerButton->OnClicked().AddUObject(this, &ThisClass::OnPromotePlayerButtonClicked);
	PromotePlayerButton->SetVisibility(ESlateVisibility::Hidden);

	KickPlayerButton->OnClicked().AddUObject(this, &ThisClass::OnKickPlayerButtonClicked);
	KickPlayerButton->SetVisibility(ESlateVisibility::Hidden);

	ServiceManager = UServiceManager::GetServiceManager();
	if (ServiceManager)
	{
		Auth = ServiceManager->GetAuth();

		Connect = ServiceManager->GetConnect();

		Lobby = ServiceManager->GetLobby();
		if (Lobby)
		{
			if (!Lobby->OnPromoteMemberComplete.IsBoundToObject(this)) Lobby->OnPromoteMemberComplete.AddUObject(this, &ThisClass::OnPromoteMemberComplete);
			if (!Lobby->OnKickMemberComplete.IsBoundToObject(this)) Lobby->OnKickMemberComplete.AddUObject(this, &ThisClass::OnKickMemberComplete);
		}
	}
}

void UPlayerLineButton::OnPlayerLineButtonHovered()
{
	if (Lobby == nullptr) return;

	if (UServiceManager::GetLobby()->IsOwner()
		&& UServiceManager::GetConnect()->ProductUserId != Member.ProductUserId)
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
	if (Lobby)
	{
		Lobby->PromoteMember(Member.ProductUserId);
	}
}

void UPlayerLineButton::OnPromoteMemberComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnPromoteMemberComplete %d"), bWasSuccessful);

	if (bWasSuccessful)
	{
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Promote lobby member failed!"), false);
	}
}

void UPlayerLineButton::OnKickPlayerButtonClicked()
{
	if (Lobby)
	{
		Lobby->KickMember(Member.ProductUserId);
	}
}

void UPlayerLineButton::OnKickMemberComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnKickMemberComplete %d"), bWasSuccessful);

	if (bWasSuccessful)
	{
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Kick lobby member failed!"), false);
	}
}
