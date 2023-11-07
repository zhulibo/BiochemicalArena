#include "HumanHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"

void AHumanHUD::BeginPlay()
{
	Super::BeginPlay();

	CrosshairLeft = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Textures/Crosshairs/Crosshair_Left.Crosshair_Left"));
	CrosshairRight = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Textures/Crosshairs/Crosshair_Right.Crosshair_Right"));
	CrosshairTop = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Textures/Crosshairs/Crosshair_Top.Crosshair_Top"));
	CrosshairBottom = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Textures/Crosshairs/Crosshair_Bottom.Crosshair_Bottom"));
}

void AHumanHUD::AddCharacterOverlay()
{
	APlayerController* HumanController = GetOwningPlayerController();
	if (HumanController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(HumanController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AHumanHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AHumanHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (CrosshairLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(CrosshairLeft, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (CrosshairRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(CrosshairRight, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (CrosshairTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(CrosshairTop, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (CrosshairBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(CrosshairBottom, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
	}
}

void AHumanHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}
