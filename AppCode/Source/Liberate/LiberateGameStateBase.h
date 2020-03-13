// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "World/WorldRegion.h"
#include "LiberateGameStateBase.generated.h"

UCLASS()
class LIBERATE_API ALiberateGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALiberateGameStateBase();

	FORCEINLINE int GetTotalRegions() const { return TotalRegions; }
	FORCEINLINE int GetCompletedRegions() const { return CompletedRegions; }
	FORCEINLINE TArray<AWorldRegion*> GetRegions() const { return Regions; }
	FORCEINLINE TSubclassOf<AWorldRegion> GetRegionClass() const { return RegionClass; }

	FORCEINLINE void SetTotalRegions(const int RegionCount) { TotalRegions = RegionCount; }
	FORCEINLINE void RegionCompleted() { CompletedRegions++; }
	FORCEINLINE void SetRegions(TArray<AWorldRegion*> WorldRegions) { Regions = WorldRegions; }
	
protected:
	UPROPERTY(Replicated)
		int TotalRegions;

	UPROPERTY(Replicated)
		int CompletedRegions;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		TArray<AWorldRegion*> Regions;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWorldRegion> RegionClass;
};
