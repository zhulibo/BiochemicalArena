// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define C_YELLOW FColor(238, 119, 0, 255)
#define C_RED FColor(187, 0, 0, 255)
#define C_GREEN FColor(0, 187, 0, 255)
#define C_WHITE FColor(255, 255, 255, 255)
#define C_BLUE FColor(0, 120, 255, 255)

#define ECC_Team1SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Team2SkeletalMesh ECollisionChannel::ECC_GameTraceChannel2

#define LOBBY_SERVERNAME FName(TEXT("ServerName"))
#define LOBBY_MODENAME FName(TEXT("ModeName"))
#define LOBBY_MAPNAME FName(TEXT("MapName"))

#define LOBBY_LOBBYNAME FName(TEXT("LobbyName"))
#define LOBBY_GAMEMODE FName(TEXT("GameMode"))

#define LOBBY_SESSIONID FName(TEXT("SessionId"))
#define LOBBY_STATUS FName(TEXT("Status"))
#define LOBBY_MEMBER_TEAM FName(TEXT("Team"))
#define LOBBY_MEMBER_PLAYERNAME FName(TEXT("PlayerName"))
#define LOBBY_MEMBER_BISREADY FName(TEXT("bIsReady"))
#define LOBBY_MEMBER_MSG FName(TEXT("Msg"))

#define TAG_Mutant_SKILL_CD FName(TEXT("Mutant.Skill.CD"))
#define TAG_MUTANT_CHANGE_ACTIVE FName(TEXT("Mutant.Change.Active"))

#define DR_HumanCharacterMain FName(TEXT("DR_HumanCharacterMain"))
#define DR_MutantCharacterMain FName(TEXT("DR_MutantCharacterMain"))
#define DR_EquipmentMain FName(TEXT("DR_EquipmentMain"))
#define DR_WeaponData FName(TEXT("DR_WeaponData"))
#define DR_MeleeData FName(TEXT("DR_MeleeData"))
#define DR_EquipmentRecoil FName(TEXT("DR_EquipmentRecoil"))
