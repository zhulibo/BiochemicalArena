#pragma once

#include "CoreMinimal.h"

#define COLOR_MAIN FColor(238, 119, 0, 255)
#define COLOR_HUMAN FColor(187, 0, 0, 255)
#define COLOR_MUTANT FColor(0, 187, 0, 255)
#define COLOR_WHITE FColor(255, 255, 255, 255)

#define ECC_Team1SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Team2SkeletalMesh ECollisionChannel::ECC_GameTraceChannel2

#define LOBBY_LOBBYNAME FName(TEXT("LobbyName"))
#define LOBBY_GAMEMODE FName(TEXT("GameMode"))
#define LOBBY_MAPNAME FName(TEXT("MapName"))
#define LOBBY_SESSIONID FName(TEXT("SessionId"))
#define LOBBY_STATUS FName(TEXT("Status"))
#define LOBBY_MEMBER_TEAM FName(TEXT("Team"))
#define LOBBY_MEMBER_PLAYERNAME FName(TEXT("PlayerName"))
#define LOBBY_MEMBER_BISREADY FName(TEXT("bIsReady"))
#define LOBBY_MEMBER_MSG FName(TEXT("Msg"))
