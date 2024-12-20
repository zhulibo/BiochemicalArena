#include "Interact.h"

#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "Components/ProgressBar.h"

void UInteract::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ProgressBar->SetFillColorAndOpacity(C_GREEN);

	if (ABaseController* BaseController = Cast<ABaseController>(GetOwningPlayer()))
	{
		BaseController->OnInteractStarted.AddUObject(this, &ThisClass::OnInteractStarted);
		BaseController->OnInteractEnded.AddUObject(this, &ThisClass::OnInteractEnded);
	}
}

void UInteract::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInteract::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ProgressBar->GetPercent() < 3)
	{
		ProgressBar->SetPercent(ProgressBar->GetPercent() + InDeltaTime / 3.f);
	}
}

void UInteract::OnInteractStarted()
{
	ProgressBar->SetPercent(0.f);
	ProgressBar->SetVisibility(ESlateVisibility::Visible);
}

void UInteract::OnInteractEnded()
{
	ProgressBar->SetVisibility(ESlateVisibility::Hidden);
	ProgressBar->SetPercent(0.f);
}

