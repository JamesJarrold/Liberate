// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShake.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "PlayerCamera.generated.h"

UCLASS()
class LIBERATE_API ULiberatePlayerCamera : public UCameraComponent
{
	GENERATED_BODY()

public:
	ULiberatePlayerCamera();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		FORCEINLINE TSubclassOf<UCameraShake> const GetRunningCameraShake() { return RunningShake; }

	UFUNCTION(BlueprintCallable)
		FORCEINLINE TSubclassOf<UCameraShake> const GetExplosionCameraShake() { return ExplosionShake; }

	UFUNCTION()
		void ActivateBlastAbilityAnimation();
	
protected:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
		TSubclassOf<UCameraShake> RunningShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
		TSubclassOf<UCameraShake> ExplosionShake;

	UPROPERTY(EditAnywhere)
		UCurveFloat* BlastAbilityCameraAngleCurve;

	UPROPERTY(EditAnywhere)
		UCurveVector* BlastAbilityCameraPositionCurve;

	float TimelineValue;
	float CameraAngleValue;
	FVector CameraPositionValue;
	FTimeline BlastAbilityCameraTimeline;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void PlayBlastAbilityCameraAnimation();
};
