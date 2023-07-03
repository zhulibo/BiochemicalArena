// Copyright Epic Games, Inc. All Rights Reserved.

#include "InfectGameMode.h"

void AInfectGameMode::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);
	// 显示调试消息五秒
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is InfectGameMode!"));
}
