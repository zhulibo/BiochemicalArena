#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "OverheadWidget.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UOverheadWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	friend class ABaseCharacter;

	void SetPlayerNameTeamColor();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY()
	class ABaseGameState* BaseGameState;

	// 拥有此OverheadWidget的角色
	UPROPERTY()
	ABaseCharacter* BaseCharacter;
	UPROPERTY()
	class ABasePlayerState* BasePlayerState;

	// 本地角色
	UPROPERTY()
	class ABaseController* LocalBaseController;
	UPROPERTY()
	ABasePlayerState* LocalBasePlayerState;
	UPROPERTY()
	ABaseCharacter* LocalBaseCharacter;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;
	void SetPlayerName();
	FTimerHandle TimerHandle;
	void ShowPlayerName();

};
