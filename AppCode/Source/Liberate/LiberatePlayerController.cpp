// Fill out your copyright notice in the Description page of Project Settings.


#include "LiberatePlayerController.h"
#include "LiberateCharacter.h" 
#include "LiberatePlayerState.h"

void ALiberatePlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	ALiberatePlayerState* CurrPlayerState = GetPlayerState<ALiberatePlayerState>();
	ALiberateCharacter* OwningPlayer = Cast<ALiberateCharacter>(GetCharacter());
	CurrPlayerState->InitialisePlayerSkills(OwningPlayer);
}
