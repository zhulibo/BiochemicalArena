#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HumanHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	class UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;

};

UCLASS()
class BIOCHEMICALARENA_API AHumanHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> AnnouncementClass;
	UPROPERTY()
	class UAnnouncement* Announcement;
	void AddAnnouncement();

protected:
	virtual void BeginPlay() override;

private:
	FHUDPackage HUDPackage;
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }

};
