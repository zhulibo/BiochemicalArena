#include "Crosshair.h"

#include "CommonBorder.h"
#include "Components/CanvasPanelSlot.h"

void UCrosshair::NativeConstruct()
{
	Super::NativeConstruct();

	Crosshair_L_Slot = Cast<UCanvasPanelSlot>(Crosshair_L->Slot);
	Crosshair_R_Slot = Cast<UCanvasPanelSlot>(Crosshair_R->Slot);
	Crosshair_T_Slot = Cast<UCanvasPanelSlot>(Crosshair_T->Slot);
	Crosshair_B_Slot = Cast<UCanvasPanelSlot>(Crosshair_B->Slot);
}

void UCrosshair::SetCrosshairSpread(float CrosshairSpread)
{
	if (Crosshair_L_Slot)
	{
		Crosshair_L_Slot->SetPosition(FVector2D(-CrosshairSpread, 0.f));
	}
	if (Crosshair_R_Slot)
	{
		Crosshair_R_Slot->SetPosition(FVector2D(CrosshairSpread, 0.f));
	}
	if (Crosshair_T_Slot)
	{
		Crosshair_T_Slot->SetPosition(FVector2D(0.f, -CrosshairSpread));
	}
	if (Crosshair_B_Slot)
	{
		Crosshair_B_Slot->SetPosition(FVector2D(0.f, CrosshairSpread));
	}
}
