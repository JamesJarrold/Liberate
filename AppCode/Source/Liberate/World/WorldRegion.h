// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Enemies/EnemyBase.h"
#include "RegionInfo.h"
#include "WorldRegion.generated.h"

//Delegate for controlling behaviour when a region is cleared
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegionCleared, FRegionInfo, RegionClearedData);

UCLASS()
class LIBERATE_API AWorldRegion : public ATriggerBox
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldRegion();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		FORCEINLINE TEnumAsByte<E_REGION_DIFFICULTY> GetRegionDifficulty() const { return RegionData.RegionDifficulty; }

	UFUNCTION()
		FORCEINLINE float GetPercentageCompleted() const { return RegionData.PercentageCompleted; }

	UFUNCTION()
		FORCEINLINE FRegionInfo GetRegionData() const { return RegionData; }

	UFUNCTION()
		void EnemyDeath(AActor* Act);

	UPROPERTY(BlueprintAssignable)
		FRegionCleared OnRegionCleared;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetRandomSpawnLocation();

	UFUNCTION()
		void InitialiseRegion();

	UFUNCTION()
		void UpdateMapPulseEffect();

	UFUNCTION()
		void OnPlayerEnterRegion(class AActor*OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
		void UpdateOverlayColour();

	UFUNCTION()
		void SpawnEnemies();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMeshComponent* RegionOverlay;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
		FRegionInfo RegionData;

	UPROPERTY(EditAnywhere, Category = Enemies)
		TArray<TSubclassOf<AEnemyBase>> EnemyTypes;

	UPROPERTY()
		TArray<AEnemyBase*> Enemies;

	UPROPERTY(BlueprintReadWrite)
		bool SelectedInWorldMap;

	UPROPERTY()
		bool ColourPulseDirection;

	UPROPERTY()
		float CurrentOpacity;
};
