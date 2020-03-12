// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "LiberateGameMode.h"
#include "LiberateGameStateBase.h"
#include "LiberateCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "UnrealNetwork.h"

ALiberateGameMode::ALiberateGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_LiberatePlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ALiberateGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitialiseRegions();
}

void ALiberateGameMode::InitialiseRegions()
{
	if (Role == ROLE_Authority)
	{
		ALiberateGameStateBase* CurrGameState = GetWorld()->GetGameState<ALiberateGameStateBase>();
		TArray<AWorldRegion*> Regions = CurrGameState->GetRegions();

		TArray<AActor*> RegionActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), CurrGameState->GetRegionClass(), RegionActors);

		for (auto it : RegionActors)
		{
			AWorldRegion* region = Cast<AWorldRegion>(it);
			region->OnRegionCleared.AddDynamic(this, &ALiberateGameMode::RegionCleared);
			Regions.Add(region);
		}

		CurrGameState->SetRegions(Regions);
		CurrGameState->SetTotalRegions(Regions.Num());
	}
}

void ALiberateGameMode::RegionCleared(FRegionInfo RegionData)
{
	if (Role == ROLE_Authority)
	{
		ALiberateGameStateBase* CurrGameState = GetWorld()->GetGameState<ALiberateGameStateBase>();
		CurrGameState->RegionCompleted();

		if (IsPlanetLiberated())
		{
			///JAMJAR TODO: Show the Planet Liberated HUD overlay -> fade out -> display end of game screen 

		}
	}
}

bool ALiberateGameMode::IsPlanetLiberated()
{
	if (Role == ROLE_Authority)
	{
		ALiberateGameStateBase* CurrGameState = GetWorld()->GetGameState<ALiberateGameStateBase>();

		if (CurrGameState->GetCompletedRegions() == CurrGameState->GetTotalRegions())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}
