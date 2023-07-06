#include "Crosshair.h"

void ACrosshair::DrawHUD()
{
	Super::DrawHUD();

	if (CrosshairTexture)
	{
		// 找出我们的画布的中心点
		FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// 偏移纹理大小的一半，以便纹理中心与画布中心对齐
		FVector2D CrossHairDrawPosition(Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f));

		// 在中心点绘制十字准星
		FCanvasTileItem TileItem(CrossHairDrawPosition, CrosshairTexture->GetResource(), FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}
}
