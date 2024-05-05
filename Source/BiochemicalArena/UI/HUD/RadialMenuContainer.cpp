#include "RadialMenuContainer.h"
#include "CommonTextBlock.h"
#include "RadialMenu.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"

void URadialMenuContainer::ShowRadialMenu()
{
	RadialMenuEquipment->SetVisibility(ESlateVisibility::Visible);
	RadialMenuRadio->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuPaint->SetVisibility(ESlateVisibility::Hidden);

	TitleEquipment->SetColorAndOpacity(ColorMutate);
	TitleRadio->SetColorAndOpacity(FColor::White);
	TitlePaint->SetColorAndOpacity(FColor::White);

	ActiveRadialMenuIndex = 1;

	SetVisibility(ESlateVisibility::Visible);
}

void URadialMenuContainer::ChangeRadialMenu()
{
	if (ActiveRadialMenuIndex == 1)
	{
		RadialMenuEquipment->SetVisibility(ESlateVisibility::Hidden);
		RadialMenuRadio->SetVisibility(ESlateVisibility::Visible);

		TitleEquipment->SetColorAndOpacity(FColor::White);
		TitleRadio->SetColorAndOpacity(ColorMutate);

		ActiveRadialMenuIndex = 2;
	}
	else if (ActiveRadialMenuIndex == 2)
	{
		RadialMenuRadio->SetVisibility(ESlateVisibility::Hidden);
		RadialMenuPaint->SetVisibility(ESlateVisibility::Visible);

		TitleRadio->SetColorAndOpacity(FColor::White);
		TitlePaint->SetColorAndOpacity(ColorMutate);

		ActiveRadialMenuIndex = 3;
	}
	else if (ActiveRadialMenuIndex == 3)
	{
		RadialMenuPaint->SetVisibility(ESlateVisibility::Hidden);
		RadialMenuEquipment->SetVisibility(ESlateVisibility::Visible);

		TitlePaint->SetColorAndOpacity(FColor::White);
		TitleEquipment->SetColorAndOpacity(ColorMutate);

		ActiveRadialMenuIndex = 1;
	}
}

void URadialMenuContainer::SelectRadialMenu(double X, double Y)
{
	if (GetActiveRadialMenu() == nullptr) return;

	// UE_LOG(LogTemp, Warning, TEXT("Select %f, %f"), X, Y);
	if (X * X + Y * Y > .6)
	{
		GetActiveRadialMenu()->Item1->SetColorAndOpacity(FColor::White);
		GetActiveRadialMenu()->Item2->SetColorAndOpacity(FColor::White);
		GetActiveRadialMenu()->Item3->SetColorAndOpacity(FColor::White);
		GetActiveRadialMenu()->Item4->SetColorAndOpacity(FColor::White);
		GetActiveRadialMenu()->Item5->SetColorAndOpacity(FColor::White);
		GetActiveRadialMenu()->Item6->SetColorAndOpacity(FColor::White);
		GetActiveRadialMenu()->Item7->SetColorAndOpacity(FColor::White);
		GetActiveRadialMenu()->Item8->SetColorAndOpacity(FColor::White);

		float Angle = FMath::Atan2(Y, X) * 180 / PI;
		// UE_LOG(LogTemp, Warning, TEXT("Angle %f"), Angle);
		if (Angle > -112.5 && Angle <= -67.5)
		{
			GetActiveRadialMenu()->Item1->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 1;
		}
		else if (Angle > -67.5 && Angle <= -22.5)
		{
			GetActiveRadialMenu()->Item2->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 2;
		}
		else if (Angle > -22.5 && Angle <= 22.5)
		{
			GetActiveRadialMenu()->Item3->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 3;
		}
		else if (Angle > 22.5 && Angle <= 67.5)
		{
			GetActiveRadialMenu()->Item4->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 4;
		}
		else if (Angle > 67.5 && Angle <= 112.5)
		{
			GetActiveRadialMenu()->Item5->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 5;
		}
		else if (Angle > 112.5 && Angle <= 157.5)
		{
			GetActiveRadialMenu()->Item6->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 6;
		}
		else if (Angle > 157.5 || Angle <= -157.5)
		{
			GetActiveRadialMenu()->Item7->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 7;
		}
		else if (Angle > -157.5 && Angle <= -112.5)
		{
			GetActiveRadialMenu()->Item8->SetColorAndOpacity(ColorMutate);
			GetActiveRadialMenu()->SelectedItem = 8;
		}
	}
}

void URadialMenuContainer::CloseRadialMenu()
{
	if (GetActiveRadialMenu())
	{
		if (ActiveRadialMenuIndex == 1)
		{
			AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());
			if (HumanCharacter)
			{
				if (GetActiveRadialMenu()->SelectedItem == 1)
				{
					HumanCharacter->SwapPrimaryEquipmentButtonPressed();
				}
				else if (GetActiveRadialMenu()->SelectedItem == 2)
				{
					HumanCharacter->SwapSecondaryEquipmentButtonPressed();
				}
				else if (GetActiveRadialMenu()->SelectedItem == 3)
				{
					HumanCharacter->SwapMeleeEquipmentButtonPressed();
				}
				else if (GetActiveRadialMenu()->SelectedItem == 4)
				{
					HumanCharacter->SwapThrowingEquipmentButtonPressed();
				}
			}

			// AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn());
			// if (MutantCharacter)
			// {
			// 	MutantCharacter->SelectCharacter(GetActiveRadialMenu()->SelectedItem);
			// }
		}
		else if (ActiveRadialMenuIndex == 2)
		{
			// AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());
			// if (HumanCharacter)
			// {
			// 	HumanCharacter->SendRadio(GetActiveRadialMenu()->SelectedItem);
			// }
			//
			// AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn());
			// if (MutantCharacter)
			// {
			// 	MutantCharacter->SendRadio(GetActiveRadialMenu()->SelectedItem);
			// }
		}
		else if (ActiveRadialMenuIndex == 3)
		{
			// AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());
			// if (HumanCharacter)
			// {
			// 	HumanCharacter->SprayPaint(GetActiveRadialMenu()->SelectedItem);
			// }
			//
			// AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn());
			// if (MutantCharacter)
			// {
			// 	MutantCharacter->SprayPaint(GetActiveRadialMenu()->SelectedItem);
			// }
		}
	}

	SetVisibility(ESlateVisibility::Hidden);
}

URadialMenu* URadialMenuContainer::GetActiveRadialMenu()
{
	switch (ActiveRadialMenuIndex)
	{
	case 1:
		return RadialMenuEquipment;
	case 2:
		return RadialMenuRadio;
	case 3:
		return RadialMenuPaint;
	default:
		return nullptr;
	}
}
