// Fill out your copyright notice in the Description page of Project Settings.


#include "LiberateGameStateBase.h"
#include "UnrealNetwork.h"

ALiberateGameStateBase::ALiberateGameStateBase()
{
	bNetLoadOnClient = true;

	TotalRegions = 0;
	CompletedRegions = 0;
}

void ALiberateGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALiberateGameStateBase, TotalRegions);
	DOREPLIFETIME(ALiberateGameStateBase, CompletedRegions);
	DOREPLIFETIME(ALiberateGameStateBase, Regions);
}
