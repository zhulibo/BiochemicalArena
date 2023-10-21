#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetLocalRole();
	// ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}

	FString DisplayString = FString::Printf(TEXT("%s"), *Role);
	SetDisplayText(DisplayString);
}

void UOverheadWidget::SetDisplayText(FString DisplayString)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(DisplayString));
	}
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
