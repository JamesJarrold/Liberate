// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"
#include "WorldMap.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LIBERATE_API UWorldMap : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWorldMap();

	UFUNCTION(BlueprintCallable)
		void InitialiseMap(ACharacter* PlayerRef);

	UFUNCTION(BlueprintCallable)
		float GetMapHeight();

	UFUNCTION(BlueprintCallable)
		float GetMapWidth();

	UFUNCTION(BlueprintCallable)
		FVector2D WorldSpaceToMapSpace(const FVector& Location);

	UFUNCTION(BlueprintCallable)
		FVector MapSpaceToWorldSpace(const FVector2D& MapLocation, const FVector2D& MapSize);

	UFUNCTION(BlueprintCallable)
		FVector2D WorldLocationToMapLocation(const FVector& Location, const FVector2D& MapOffset, const FVector2D& MapSize);

	UFUNCTION(BlueprintCallable)
		FVector MapLocationToWorldLocation(FVector2D MapLocation, const FVector2D& MapOffset, const FVector2D& MapSize);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ATargetPoint* TopLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ATargetPoint* BottomRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ACharacter* Player;
};
