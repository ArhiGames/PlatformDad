// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformDadGameMode.h"
#include "PlatformDadCharacter.h"
#include "UObject/ConstructorHelpers.h"

APlatformDadGameMode::APlatformDadGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
