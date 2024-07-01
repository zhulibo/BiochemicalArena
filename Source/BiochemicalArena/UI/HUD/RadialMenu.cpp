#include "RadialMenu.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/BiochemicalArena.h"

FString URadialMenu::GetSelectedItem()
{
	if (SelectedItemIndex == 1) return Item1->GetText().ToString();
	if (SelectedItemIndex == 2) return Item2->GetText().ToString();
	if (SelectedItemIndex == 3) return Item3->GetText().ToString();
	if (SelectedItemIndex == 3) return Item3->GetText().ToString();
	if (SelectedItemIndex == 4) return Item4->GetText().ToString();
	if (SelectedItemIndex == 5) return Item5->GetText().ToString();
	if (SelectedItemIndex == 6) return Item6->GetText().ToString();
	if (SelectedItemIndex == 7) return Item7->GetText().ToString();
	if (SelectedItemIndex == 8) return Item8->GetText().ToString();

	return "";
}

void URadialMenu::SetSelectedItem(int32 NewSelectedItemIndex)
{
	if (SelectedItemIndex == 1) Item1->SetColorAndOpacity(COLOR_WHITE);
	else if (SelectedItemIndex == 2) Item2->SetColorAndOpacity(COLOR_WHITE);
	else if (SelectedItemIndex == 3) Item3->SetColorAndOpacity(COLOR_WHITE);
	else if (SelectedItemIndex == 4) Item4->SetColorAndOpacity(COLOR_WHITE);
	else if (SelectedItemIndex == 5) Item5->SetColorAndOpacity(COLOR_WHITE);
	else if (SelectedItemIndex == 6) Item6->SetColorAndOpacity(COLOR_WHITE);
	else if (SelectedItemIndex == 7) Item7->SetColorAndOpacity(COLOR_WHITE);
	else if (SelectedItemIndex == 8) Item8->SetColorAndOpacity(COLOR_WHITE);

	if (NewSelectedItemIndex == 1) Item1->SetColorAndOpacity(COLOR_MUTANT);
	else if (NewSelectedItemIndex == 2) Item2->SetColorAndOpacity(COLOR_MUTANT);
	else if (NewSelectedItemIndex == 3) Item3->SetColorAndOpacity(COLOR_MUTANT);
	else if (NewSelectedItemIndex == 4) Item4->SetColorAndOpacity(COLOR_MUTANT);
	else if (NewSelectedItemIndex == 5) Item5->SetColorAndOpacity(COLOR_MUTANT);
	else if (NewSelectedItemIndex == 6) Item6->SetColorAndOpacity(COLOR_MUTANT);
	else if (NewSelectedItemIndex == 7) Item7->SetColorAndOpacity(COLOR_MUTANT);
	else if (NewSelectedItemIndex == 8) Item8->SetColorAndOpacity(COLOR_MUTANT);

	SelectedItemIndex = NewSelectedItemIndex;
}
