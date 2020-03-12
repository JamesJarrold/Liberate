// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldMap.h"

// Sets default values for this component's properties
UWorldMap::UWorldMap()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UWorldMap::InitialiseMap(ACharacter* PlayerRef)
{
	Player = PlayerRef;
}

float UWorldMap::GetMapHeight()
{
	FVector TopLeftLocation = TopLeft->GetActorLocation();
	FVector BottomRightLocation = BottomRight->GetActorLocation();
	float Height = abs(BottomRightLocation.X - TopLeftLocation.X);

	return Height;
}

float UWorldMap::GetMapWidth()
{
	FVector TopLeftLocation = TopLeft->GetActorLocation();
	FVector BottomRightLocation = BottomRight->GetActorLocation();
	float Width = abs(BottomRightLocation.Y - TopLeftLocation.Y);

	return Width;
}

FVector2D UWorldMap::WorldSpaceToMapSpace(const FVector& Location)
{
	FVector TopLeftLocation = TopLeft->GetActorLocation();
	FVector BottomRightLocation = BottomRight->GetActorLocation();
	FVector2D OutLocation; 

	OutLocation.X = abs(Location.Y - TopLeftLocation.Y);
	OutLocation.Y = abs(Location.X - TopLeftLocation.X);

	return OutLocation;
}

FVector UWorldMap::MapSpaceToWorldSpace(const FVector2D& MapLocation, const FVector2D& MapSize)
{
	FVector TopLeftLocation = TopLeft->GetActorLocation();
	FVector BottomRightLocation = BottomRight->GetActorLocation();
	FVector OutLocation;

	float PosPercentageX = MapLocation.X / MapSize.X;
	float PosPercentageY = MapLocation.Y / MapSize.Y;

	OutLocation.X = TopLeftLocation.X - (GetMapHeight() * PosPercentageY);
	OutLocation.Y = TopLeftLocation.Y + (GetMapWidth() * PosPercentageX);
	OutLocation.Z = 0.f;

	return OutLocation;
}

FVector2D UWorldMap::WorldLocationToMapLocation(const FVector& Location, const FVector2D& MapOffset, const FVector2D& MapSize)
{
	FVector2D MapLocation = WorldSpaceToMapSpace(Location);

	MapLocation.X = ((MapLocation.X / GetMapWidth()) * MapSize.X) + MapOffset.X;
	MapLocation.Y = ((MapLocation.Y / GetMapHeight()) * MapSize.Y) + MapOffset.Y;

	return MapLocation;
}

FVector UWorldMap::MapLocationToWorldLocation(FVector2D MapLocation, const FVector2D& MapOffset, const FVector2D & MapSize)
{
	MapLocation.X -= MapOffset.X;
	MapLocation.Y -= MapOffset.Y;

	FVector WorldLocation = MapSpaceToWorldSpace(MapLocation, MapSize);

	return WorldLocation;
}
