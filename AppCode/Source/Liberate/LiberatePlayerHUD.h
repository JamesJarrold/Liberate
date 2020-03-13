// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LiberatePlayerHUD.generated.h"

UCLASS()
class LIBERATE_API ALiberatePlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ALiberatePlayerHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;
};
