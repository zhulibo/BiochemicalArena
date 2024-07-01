#include "OverheadWidget.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

void UOverheadWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetPlayerName();

	if (BaseCharacter)
	{
		if (!BaseCharacter->IsLocallyControlled())
		{
			SetPlayerNameTeamColor();
		}
		// 本地玩家队伍改变时，更新所有玩家的队伍颜色
		else
		{
			TArray<AActor*> AllPlayers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), AllPlayers);
			for (AActor* Player : AllPlayers)
			{
				if (ABaseCharacter* IgnoreBaseCharacter = Cast<ABaseCharacter>(Player))
				{
					if (UWidgetComponent* OverheadWidget = IgnoreBaseCharacter->GetOverheadWidget())
					{
						if (UOverheadWidget* OverheadWidgetClass = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject()))
						{
							OverheadWidgetClass->SetPlayerNameTeamColor();
						}
					}
				}
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::ShowPlayerName, .2f, true, .1f);
}

// 设置用户名
void UOverheadWidget::SetPlayerName()
{
	if (BaseCharacter)
	{
		if (BasePlayerState == nullptr) BasePlayerState = Cast<ABasePlayerState>(BaseCharacter->GetPlayerState());
		if (BasePlayerState)
		{
			PlayerName->SetText(FText::FromString(BasePlayerState->GetPlayerName()));
			return;
		}
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		SetPlayerName();
	});
}

// 设置PlayerName队伍颜色
void UOverheadWidget::SetPlayerNameTeamColor()
{
	if (BaseCharacter)
	{
		if (BasePlayerState == nullptr) BasePlayerState = Cast<ABasePlayerState>(BaseCharacter->GetPlayerState());

		if (LocalBasePlayerState == nullptr)
		{
			if (GetWorld() == nullptr) return; // TODO 需要查找闪退原因
			if (LocalBaseController == nullptr) LocalBaseController = Cast<ABaseController>(GetWorld()->GetFirstPlayerController());
			if (LocalBaseController) LocalBasePlayerState = Cast<ABasePlayerState>(LocalBaseController->PlayerState);
		}

		if (BasePlayerState && LocalBasePlayerState)
		{
			// UE_LOG(LogTemp, Warning, TEXT("SetColorAndOpacity Base %d Local %d"), BasePlayerState->GetTeam(), LocalBasePlayerState->GetTeam());
			if (BasePlayerState->GetTeam() != ETeam::NoTeam && LocalBasePlayerState->GetTeam() != ETeam::NoTeam)
			{
				if (BasePlayerState->GetTeam() == LocalBasePlayerState->GetTeam())
				{
					PlayerName->SetColorAndOpacity(COLOR_WHITE);
					return;
				}
				else
				{
					PlayerName->SetColorAndOpacity(COLOR_HUMAN);
					return;
				}
			}
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("SetTimerForNextTick"));
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		SetPlayerNameTeamColor();
	});
}

// 判断是否显示PlayerName
void UOverheadWidget::ShowPlayerName()
{
	if (LocalBaseCharacter == nullptr)
	{
		if (LocalBaseController == nullptr) LocalBaseController = Cast<ABaseController>(GetWorld()->GetFirstPlayerController());
		if (LocalBaseController) LocalBaseCharacter = Cast<ABaseCharacter>(LocalBaseController->GetPawn());
	}

	if (BaseCharacter && LocalBaseCharacter && BaseCharacter != LocalBaseCharacter)
	{
		FHitResult HitResult;
		FVector Start = LocalBaseCharacter->GetCamera()->GetComponentLocation();
		FVector End = BaseCharacter->GetCamera()->GetComponentLocation();

		// 距离过远不显示PlayerName
		if (FVector::Dist(Start, End) > 3000.f)
		{
			SetVisibility(ESlateVisibility::Hidden);
			return;
		}

		// double Time1 = FPlatformTime::Seconds();

		// 射线检测玩家是否被阻挡
		FCollisionQueryParams QueryParams;
		TArray<AActor*> IgnoreActors;
		// IgnoreActors.AddUnique(LocalBaseCharacter);
		// IgnoreActors.AddUnique(BaseCharacter);
		if (BaseGameState == nullptr) BaseGameState = GetWorld()->GetGameState<ABaseGameState>();
		if (BaseGameState)
		{
			if (BaseGameState && BaseGameState->GetTeam(ETeam::Team1).Num() > 0)
			{
				for (int32 i = 0; i < BaseGameState->GetTeam(ETeam::Team1).Num(); ++i)
				{
					if (BaseGameState->GetTeam(ETeam::Team1)[i])
					{
						IgnoreActors.AddUnique(BaseGameState->GetTeam(ETeam::Team1)[i]->GetPawn());
					}
				}
			}
			if (BaseGameState && BaseGameState->GetTeam(ETeam::Team2).Num() > 0)
			{
				for (int32 i = 0; i < BaseGameState->GetTeam(ETeam::Team2).Num(); ++i)
				{
					if (BaseGameState->GetTeam(ETeam::Team2)[i])
					{
						IgnoreActors.AddUnique(BaseGameState->GetTeam(ETeam::Team2)[i]->GetPawn());
					}
				}
			}
		}
		QueryParams.AddIgnoredActors(IgnoreActors);

		// double Time2 = FPlatformTime::Seconds();

		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

		// double Time3 = FPlatformTime::Seconds();
		// UE_LOG(LogTemp, Warning, TEXT("IgnoreActors %f LineTraceSingleByChannel %f"), Time2 - Time1, Time3 - Time2);

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
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	Super::NativeDestruct();
}
