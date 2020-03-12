// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"

ULiberatePlayerCamera::ULiberatePlayerCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void ULiberatePlayerCamera::BeginPlay()
{
	Super::BeginPlay();

	if (BlastAbilityCameraAngleCurve && BlastAbilityCameraPositionCurve)
	{
		FOnTimelineFloat TimelineCallbackFloat;
		FOnTimelineVector TimelineCallbackVector;

		TimelineCallbackFloat.BindUFunction(this, FName("PlayBlastAbilityCameraAnimation"));
		TimelineCallbackVector.BindUFunction(this, FName("PlayBlastAbilityCameraAnimation"));
		BlastAbilityCameraTimeline.AddInterpFloat(BlastAbilityCameraAngleCurve, TimelineCallbackFloat);
		BlastAbilityCameraTimeline.AddInterpVector(BlastAbilityCameraPositionCurve, TimelineCallbackVector);
	}
}

void ULiberatePlayerCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	BlastAbilityCameraTimeline.TickTimeline(DeltaTime);
}

void ULiberatePlayerCamera::PlayBlastAbilityCameraAnimation()
{
	TimelineValue = BlastAbilityCameraTimeline.GetPlaybackPosition();
	CameraAngleValue = BlastAbilityCameraAngleCurve->GetFloatValue(TimelineValue);
	CameraPositionValue = BlastAbilityCameraPositionCurve->GetVectorValue(TimelineValue);

	FQuat CameraRotation = FQuat(FRotator(CameraAngleValue, 0.f, 0.f));

	SetRelativeLocation(CameraPositionValue);
	SetRelativeRotation(CameraRotation);
}

void ULiberatePlayerCamera::ActivateBlastAbilityAnimation()
{
	BlastAbilityCameraTimeline.PlayFromStart();
}