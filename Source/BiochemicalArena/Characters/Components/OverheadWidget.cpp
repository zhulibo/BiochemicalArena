#include "OverheadWidget.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerStates/HumanState.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

void UOverheadWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 等待所有玩家加载完毕再设置PlayerName
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::SetPlayerName, 1.f);

	// 判断角色是否被阻挡
	FTimerHandle TimerHandle2;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &ThisClass::RefreshPlayerName, .2f, true, 1.f);
}

void UOverheadWidget::SetPlayerName()
{
	if (HumanCharacter)
	{
		AHumanState* LocalHumanState = Cast<AHumanState>(GetOwningPlayerState());
		AHumanState* HumanState = Cast<AHumanState>(HumanCharacter->GetPlayerState());
		if (LocalHumanState && HumanState)
		{
			PlayerName->SetText(FText::FromString(HumanState->GetPlayerName()));

			if (LocalHumanState->GetTeam() != HumanState->GetTeam())
			{
				PlayerName->SetColorAndOpacity(FLinearColor::Red);
			}
		}
	}
}

void UOverheadWidget::RefreshPlayerName()
{
	if (LocalHumanCharacter == nullptr) LocalHumanCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());
	if (LocalHumanCharacter && HumanCharacter && LocalHumanCharacter != HumanCharacter)
	{
		FHitResult HitResult;
		FVector Start = LocalHumanCharacter->GetCamera()->GetComponentLocation();
		FVector End = HumanCharacter->GetCamera()->GetComponentLocation();

		if (FVector::Dist(Start, End) > 3000.f)
		{
			SetVisibility(ESlateVisibility::Hidden); // 距离过远不显示PlayerName
			return;
		}

		FCollisionQueryParams QueryParams;
		TArray<AActor*> IgnoreActor;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), IgnoreActor);
		QueryParams.AddIgnoredActors(IgnoreActor); // 忽略所有玩家
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

		if (HitResult.bBlockingHit)
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}
