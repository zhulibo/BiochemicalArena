#include "RadialMenuContainer.h"
#include "CommonTextBlock.h"
#include "RadialMenu.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/CharacterType.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

void URadialMenuContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->OnRoundStarted.AddUObject(this, &ThisClass::OnRoundStarted);
	}
}

// 显示轮盘
void URadialMenuContainer::ShowRadialMenu()
{
	HumanCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());
	if (HumanCharacter == nullptr) MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn());

	if (HumanCharacter)
	{
		TitleEquipment->SetColorAndOpacity(COLOR_MUTANT);
		TitleEquipment->SetVisibility(ESlateVisibility::Visible);

		TitleMutant->SetColorAndOpacity(COLOR_WHITE);
		TitleMutant->SetVisibility(ESlateVisibility::Collapsed);

		RadialMenuEquipment->SetVisibility(ESlateVisibility::Visible);
		RadialMenuMutant->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (MutantCharacter)
	{
		TitleEquipment->SetColorAndOpacity(COLOR_WHITE);
		TitleEquipment->SetVisibility(ESlateVisibility::Collapsed);

		TitleMutant->SetColorAndOpacity(COLOR_MUTANT);
		TitleMutant->SetVisibility(ESlateVisibility::Visible);

		RadialMenuEquipment->SetVisibility(ESlateVisibility::Hidden);
		RadialMenuMutant->SetVisibility(ESlateVisibility::Visible);
	}

	TitleRadio->SetColorAndOpacity(COLOR_WHITE);
	TitlePaint->SetColorAndOpacity(COLOR_WHITE);

	RadialMenuRadio->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuPaint->SetVisibility(ESlateVisibility::Hidden);

	ActiveRadialMenuIndex = 1;

	SetSelectedItem();

	SetVisibility(ESlateVisibility::Visible);
}

// 设置轮盘选中项
void URadialMenuContainer::SetSelectedItem()
{
	if (HumanCharacter)
	{
		if (HumanCharacter->GetCombatComponent() && HumanCharacter->GetCombatComponent()->GetCurEquipment())
		{
			switch (HumanCharacter->GetCombatComponent()->GetCurEquipment()->GetEquipmentType())
			{
			case EEquipmentType::Primary:
				RadialMenuEquipment->SetSelectedItem(1);
				RadialMenuEquipment->LastSelectedItemIndex = 1;
				break;
			case EEquipmentType::Secondary:
				RadialMenuEquipment->SetSelectedItem(2);
				RadialMenuEquipment->LastSelectedItemIndex = 2;
				break;
			case EEquipmentType::Melee:
				RadialMenuEquipment->SetSelectedItem(3);
				RadialMenuEquipment->LastSelectedItemIndex = 3;
				break;
			case EEquipmentType::Throwing:
				RadialMenuEquipment->SetSelectedItem(4);
				RadialMenuEquipment->LastSelectedItemIndex = 4;
				break;
			}
		}
	}
	else if (MutantCharacter)
	{
		switch (MutantCharacter->MutantCharacterName)
		{
		case EMutantCharacterName::Tank:
			RadialMenuMutant->SetSelectedItem(1);
			RadialMenuMutant->LastSelectedItemIndex = 1;
			break;
		case EMutantCharacterName::Ghost:
			RadialMenuMutant->SetSelectedItem(2);
			RadialMenuMutant->LastSelectedItemIndex = 2;
			break;
		}
	}
}

// 切换轮盘
void URadialMenuContainer::ChangeRadialMenu()
{
	if (ActiveRadialMenuIndex == 1)
	{
		if (HumanCharacter)
		{
			RadialMenuEquipment->SetVisibility(ESlateVisibility::Hidden);
			TitleEquipment->SetColorAndOpacity(COLOR_WHITE);
		}
		else if (MutantCharacter)
		{
			RadialMenuMutant->SetVisibility(ESlateVisibility::Hidden);
			TitleMutant->SetColorAndOpacity(COLOR_WHITE);
		}

		RadialMenuRadio->SetVisibility(ESlateVisibility::Visible);
		TitleRadio->SetColorAndOpacity(COLOR_MUTANT);

		ActiveRadialMenuIndex = 2;
	}
	else if (ActiveRadialMenuIndex == 2)
	{
		RadialMenuRadio->SetVisibility(ESlateVisibility::Hidden);
		TitleRadio->SetColorAndOpacity(COLOR_WHITE);

		RadialMenuPaint->SetVisibility(ESlateVisibility::Visible);
		TitlePaint->SetColorAndOpacity(COLOR_MUTANT);

		ActiveRadialMenuIndex = 3;
	}
	else if (ActiveRadialMenuIndex == 3)
	{
		RadialMenuPaint->SetVisibility(ESlateVisibility::Hidden);
		TitlePaint->SetColorAndOpacity(COLOR_WHITE);

		if (HumanCharacter)
		{
			RadialMenuEquipment->SetVisibility(ESlateVisibility::Visible);
			TitleEquipment->SetColorAndOpacity(COLOR_MUTANT);
		}
		else if (MutantCharacter)
		{
			RadialMenuMutant->SetVisibility(ESlateVisibility::Visible);
			TitleMutant->SetColorAndOpacity(COLOR_MUTANT);
		}

		ActiveRadialMenuIndex = 1;
	}
}

// 选择轮盘项
void URadialMenuContainer::SelectRadialMenu(double X, double Y)
{
	// UE_LOG(LogTemp, Warning, TEXT("Select %f, %f"), X, Y);
	if (X * X + Y * Y < .6) return;

	if (URadialMenu* RadialMenu = GetActiveRadialMenu())
	{
		float Angle = FMath::Atan2(Y, X) * 180 / PI;
		// UE_LOG(LogTemp, Warning, TEXT("Angle %f"), Angle);

		if (Angle > -112.5 && Angle <= -67.5)
		{
			RadialMenu->SetSelectedItem(1);
		}
		else if (Angle > -67.5 && Angle <= -22.5)
		{
			RadialMenu->SetSelectedItem(2);
		}
		else if (Angle > -22.5 && Angle <= 22.5)
		{
			RadialMenu->SetSelectedItem(3);
		}
		else if (Angle > 22.5 && Angle <= 67.5)
		{
			RadialMenu->SetSelectedItem(4);
		}
		else if (Angle > 67.5 && Angle <= 112.5)
		{
			RadialMenu->SetSelectedItem(5);
		}
		else if (Angle > 112.5 && Angle <= 157.5)
		{
			RadialMenu->SetSelectedItem(6);
		}
		else if (Angle > 157.5 || Angle <= -157.5)
		{
			RadialMenu->SetSelectedItem(7);
		}
		else if (Angle > -157.5 && Angle <= -112.5)
		{
			RadialMenu->SetSelectedItem(8);
		}
	}
}

// 关闭轮盘
void URadialMenuContainer::CloseRadialMenu()
{
	if (ActiveRadialMenuIndex == 1)
	{
		// 人类
		if (HumanCharacter)
		{
			if (RadialMenuEquipment->LastSelectedItemIndex != RadialMenuEquipment->SelectedItemIndex)
			{
				if (RadialMenuEquipment->SelectedItemIndex == 1)
				{
					HumanCharacter->SwapPrimaryEquipmentButtonPressed();
				}
				else if (RadialMenuEquipment->SelectedItemIndex == 2)
				{
					HumanCharacter->SwapSecondaryEquipmentButtonPressed();
				}
				else if (RadialMenuEquipment->SelectedItemIndex == 3)
				{
					HumanCharacter->SwapMeleeEquipmentButtonPressed();
				}
				else if (RadialMenuEquipment->SelectedItemIndex == 4)
				{
					HumanCharacter->SwapThrowingEquipmentButtonPressed();
				}
			}
		}
		// 突变体
		else if (MutantCharacter)
		{
			if (RadialMenuMutant->LastSelectedItemIndex != RadialMenuMutant->SelectedItemIndex)
			{
				FString SelectedItem = RadialMenuMutant->GetSelectedItem();
				if (SelectedItem != "-1")
				{
					MutantCharacter->ServerSelectCharacter(SelectedItem);

					DisableSelectCharacter();
					GetWorld()->GetTimerManager().ClearTimer(DisableTimerHandle);
				}
			}
		}
	}
	else if (ActiveRadialMenuIndex == 2)
	{
		// // 人类
		// if (HumanCharacter)
		// {
		// 	HumanCharacter->SendRadio(RadialMenuRadio->SelectedItemIndex);
		// }
		// // 突变体
		// else if (MutantCharacter)
		// {
		// 	MutantCharacter->SendRadio(RadialMenuRadio->SelectedItemIndex);
		// }
	}
	else if (ActiveRadialMenuIndex == 3)
	{
		// // 人类
		// if (HumanCharacter)
		// {
		// 	HumanCharacter->SprayPaint(RadialMenuPaint->SelectedItemIndex);
		// }
		// // 突变体
		// else if (MutantCharacter)
		// {
		// 	MutantCharacter->SprayPaint(RadialMenuPaint->SelectedItemIndex);
		// }
	}

	SetVisibility(ESlateVisibility::Hidden);
}

// 获取活跃的轮盘
URadialMenu* URadialMenuContainer::GetActiveRadialMenu()
{
	switch (ActiveRadialMenuIndex)
	{
	case 1:
		if (HumanCharacter) return RadialMenuEquipment;
		if (MutantCharacter) return RadialMenuMutant;
	case 2:
		return RadialMenuRadio;
	case 3:
		return RadialMenuPaint;
	default:
		return nullptr;
	}
}

// 突变模式-队伍改变
void URadialMenuContainer::OnTeamChange(ETeam Team)
{
	HumanCharacter = nullptr;
	MutantCharacter = nullptr;

	if (Team == ETeam::Team1)
	{
	}
	else if (Team == ETeam::Team2)
	{
		EnableSelectCharacter();
	}
}

// 允许切换角色
void URadialMenuContainer::EnableSelectCharacter()
{
	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->EnableSelectCharacter();

		RadialMenuMutant->Item1->SetText(FText::FromString("Tank"));
		RadialMenuMutant->Item2->SetText(FText::FromString("Ghost"));
		RadialMenuMutant->Item3->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item4->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item5->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item6->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item7->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item8->SetText(FText::FromString("-1"));

		GetWorld()->GetTimerManager().SetTimer(DisableTimerHandle, this, &ThisClass::DisableSelectCharacter, 10.f);
	}
}

// 禁用切换角色
void URadialMenuContainer::DisableSelectCharacter()
{
	if (AMutationController* MutationController = Cast<AMutationController>(GetOwningPlayer()))
	{
		MutationController->DisableSelectCharacter();

		RadialMenuMutant->Item1->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item2->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item3->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item4->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item5->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item6->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item7->SetText(FText::FromString("-1"));
		RadialMenuMutant->Item8->SetText(FText::FromString("-1"));
	}
}

// 对局开始
void URadialMenuContainer::OnRoundStarted()
{
	GetWorld()->GetTimerManager().ClearTimer(DisableTimerHandle);
}
