// Copyright Epic Games, Inc. All Rights Reserved.

#include "LitUpGameMode.h"
#include "LitUpCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALitUpGameMode::ALitUpGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
