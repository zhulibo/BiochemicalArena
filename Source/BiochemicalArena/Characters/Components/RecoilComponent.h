#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RecoilComponent.generated.h"

enum class ECombatState : uint8;
enum class EEquipmentType : uint8;

UENUM(BlueprintType)
enum class ERecoilHorDirection : uint8
{
	Random UMETA(DisplayName = "Random"),
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BIOCHEMICALARENA_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URecoilComponent();
	friend class AHumanCharacter;

	void IncRecoil();

	void SetRecoilHorDirection(ERecoilHorDirection TempRecoilHorDirection);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	AHumanCharacter* HumanCharacter;

	// 输入倍率默认值
	float InputPitchScale_DEPRECATED = -2.5f;
	float InputYawScale_DEPRECATED = 2.5f;

	// 本次开火需要应用的目标后坐力
	float RecoilVertTarget = 0.f;
	float RecoilHorTarget = 0.f;
	// 上一帧的后坐力
	float RecoilVertLastTick = 0.f;
	float RecoilHorLastTick = 0.f;
	// 应用后坐力已经消耗的时间
	float RecoilIncCostTime = 0.f;
	// 总后坐力
	float RecoilVertTotal = 0.f;
	float RecoilHorTotal = 0.f;
	// 水平后坐力方向
	ERecoilHorDirection RecoilHorDirection = ERecoilHorDirection::Random;
	void PollApplyRecoil(float DeltaSeconds);

	// 回复后坐力需要的时间
	float RecoilDecTime = 0.f;
	// 回复后坐力已经消耗的时间
	float RecoilDecCostTime = 0.f;
	// 记录开始回复后坐力时的总后坐力
	float RecoilVertTotalRecover = 0.f;
	float RecoilHorTotalRecover = 0.f;
	// 上一帧的后坐力
	float RecoilVertLastTick2 = 0.f;
	float RecoilHorLastTick2 = 0.f;
	void PollRecoverRecoil(float DeltaSeconds);

public:
	float GetCurRecoilVert();
	float GetCurRecoilHor();

};
