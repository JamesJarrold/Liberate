// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LiberatePlayerController.generated.h"

UCLASS()
class LIBERATE_API ALiberatePlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlayingState() override;
	
};
