#include "RadialMenu.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/BiochemicalArena.h"

void URadialMenu::SetSelectedItem(int32 NewSelectedItemIndex)
{
	if (NewSelectedItemIndex == SelectedItemIndex) return;

	switch (SelectedItemIndex)
	{
	case 1:
		Item1->SetColorAndOpacity(COLOR_WHITE);
		break;
	case 2:
		Item2->SetColorAndOpacity(COLOR_WHITE);
		break;
	case 3:
		Item3->SetColorAndOpacity(COLOR_WHITE);
		break;
	case 4:
		Item4->SetColorAndOpacity(COLOR_WHITE);
		break;
	case 5:
		Item5->SetColorAndOpacity(COLOR_WHITE);
		break;
	case 6:
		Item6->SetColorAndOpacity(COLOR_WHITE);
		break;
	case 7:
		Item7->SetColorAndOpacity(COLOR_WHITE);
		break;
	case 8:
		Item8->SetColorAndOpacity(COLOR_WHITE);
		break;
	}

	switch (NewSelectedItemIndex)
	{
	case 1:
		Item1->SetColorAndOpacity(COLOR_MUTANT);
		break;
	case 2:
		Item2->SetColorAndOpacity(COLOR_MUTANT);
		break;
	case 3:
		Item3->SetColorAndOpacity(COLOR_MUTANT);
		break;
	case 4:
		Item4->SetColorAndOpacity(COLOR_MUTANT);
		break;
	case 5:
		Item5->SetColorAndOpacity(COLOR_MUTANT);
		break;
	case 6:
		Item6->SetColorAndOpacity(COLOR_MUTANT);
		break;
	case 7:
		Item7->SetColorAndOpacity(COLOR_MUTANT);
		break;
	case 8:
		Item8->SetColorAndOpacity(COLOR_MUTANT);
		break;
	}

	SelectedItemIndex = NewSelectedItemIndex;
}
