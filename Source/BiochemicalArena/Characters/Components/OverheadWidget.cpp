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
#include "CommonLazyImage.h"
#include "Kismet/GameplayStatics.h"

void UOverheadWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetPlayerName();

	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::TraceOverheadWidget, .2f, true, .1f);

	if (BaseCharacter)
	{
		if (!BaseCharacter->IsLocallyControlled())
		{
			InitOverheadWidget();
		}
		// 本地玩家队伍改变时，初始化本机所有玩家的OverheadWidget
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
							OverheadWidgetClass->InitOverheadWidget();
						}
					}
				}
			}
		}
	}
}

void UOverheadWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);

	Super::NativeDestruct();
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

// 判断是否显示OverheadWidget
void UOverheadWidget::TraceOverheadWidget()
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
		if (FVector::Dist(Start, End) > 2000.f)
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

void UOverheadWidget::InitOverheadWidget()
{
	if (BaseCharacter)
	{
		if (BasePlayerState == nullptr) BasePlayerState = Cast<ABasePlayerState>(BaseCharacter->GetPlayerState());

		if (LocalBasePlayerState == nullptr)
		{
			if (LocalBaseController == nullptr) LocalBaseController = GetWorld() ? Cast<ABaseController>(GetWorld()->GetFirstPlayerController()) : nullptr;
			if (LocalBaseController) LocalBasePlayerState = Cast<ABasePlayerState>(LocalBaseController->PlayerState);
		}

		if (BasePlayerState && LocalBasePlayerState)
		{
			// UE_LOG(LogTemp, Warning, TEXT("SetColorAndOpacity Base %d Local %d"), BasePlayerState->GetTeam(), LocalBasePlayerState->GetTeam());
			if (BasePlayerState->GetTeam() != ETeam::NoTeam && LocalBasePlayerState->GetTeam() != ETeam::NoTeam)
			{
				// 设置血条刻度
				if (UMaterialInstanceDynamic* MID = HealthBarLine->GetDynamicMaterial())
				{
					MID->SetScalarParameterValue(TEXT("LineNum"), GetHealthBarLineNum());
				}

				if (BasePlayerState->GetTeam() == LocalBasePlayerState->GetTeam())
				{
					// 设置名字颜色
					PlayerName->SetColorAndOpacity(C_WHITE);

					// 设置血条颜色
					if (UMaterialInstanceDynamic* MID = HealthBar->GetDynamicMaterial())
					{
						MID->SetVectorParameterValue(TEXT("TeamColor"), BaseCharacter->bIsImmune ? C_BLUE : C_WHITE);
					}
				}
				else
				{
					PlayerName->SetColorAndOpacity(C_RED);

					if (UMaterialInstanceDynamic* MID = HealthBar->GetDynamicMaterial())
					{
						MID->SetVectorParameterValue(TEXT("TeamColor"), BaseCharacter->bIsImmune ? C_BLUE : C_RED);
					}
				}

				return;
			}
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("SetTimerForNextTick"));
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		InitOverheadWidget();
	});
}

int32 UOverheadWidget::GetHealthBarLineNum()
{
	float MaxHealth = BaseCharacter->GetMaxHealth();
	// 每格血量
	float UnitHealth = MaxHealth > 1000 ? 1000 : 100;

	return FMath::FloorToInt(MaxHealth / UnitHealth);
}

void UOverheadWidget::OnMaxHealthChange(float MaxHealth)
{
	if (UMaterialInstanceDynamic* MID = HealthBarLine->GetDynamicMaterial())
	{
		MID->SetScalarParameterValue(TEXT("LineNum"), GetHealthBarLineNum());
	}
}

void UOverheadWidget::OnHealthChange(float OldHealth, float NewHealth)
{
	if (BaseCharacter == nullptr) return;
	float MaxHealth = BaseCharacter->GetMaxHealth();

	// UE_LOG(LogTemp, Warning, TEXT("OldHealth %f NewHealth %f MaxHealth %f"), OldHealth, NewHealth, MaxHealth);

	float OldValue = FMath::Clamp(OldHealth / MaxHealth, 0.f, 1.f);
	float NewValue = FMath::Clamp(NewHealth / MaxHealth, 0.f, 1.f);

	if (UMaterialInstanceDynamic* MID = HealthBar->GetDynamicMaterial())
	{
		MID->SetScalarParameterValue(TEXT("OldValue"), OldValue);
		MID->SetScalarParameterValue(TEXT("NewValue"), NewValue);

		float PlaybackSpeed = FMath::Abs(NewValue - OldValue) * 3;

		// UE_LOG(LogTemp, Warning, TEXT("OldValue %f NewValue %f PlaybackSpeed %f"), OldValue, NewValue, PlaybackSpeed);

		if (OldValue > NewValue)
		{
			PlayAnimationTimeRange(HealthDec, 1 - OldValue, 1 - NewValue, 1,  EUMGSequencePlayMode::Forward, PlaybackSpeed);
		}
		else if (OldValue < NewValue)
		{
			PlayAnimationTimeRange(HealthInc, 1 - OldValue, 1 - NewValue, 1,  EUMGSequencePlayMode::Forward, PlaybackSpeed);
		}
	}
}
