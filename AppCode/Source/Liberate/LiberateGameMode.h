// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "World/WorldRegion.h"
#include "World/RegionInfo.h"
#include "LiberateGameMode.generated.h"

UCLASS(minimalapi)
class ALiberateGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALiberateGameMode();

	virtual void BeginPlay() override;

	UFUNCTION()
		void RegionCleared(FRegionInfo RegionData);

	UFUNCTION()
		bool IsPlanetLiberated();

protected:
	UFUNCTION()
		void InitialiseRegions();
};
