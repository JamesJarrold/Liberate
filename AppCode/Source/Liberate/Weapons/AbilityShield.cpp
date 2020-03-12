// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityShield.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "LiberateCharacter.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UAbilityShield::UAbilityShield()
{
	IsShieldActive = false;
	EnergyConsumption = 50.f;
	ShieldStrength = 0.f;

	SetIsReplicated(true);
}

void UAbilityShield::ActivateAbility()
{
	IsShieldActive = true;

	ActivateAbilityEffects();

	GetWorld()->GetTimerManager().SetTimer(ShieldActiveTimer, this, &UAbilityShield::OnShieldTimerExpired, ShieldStrength);
}

void UAbilityShield::ActivateAbilityEffects()
{
	ALiberateCharacter* Player = Cast<ALiberateCharacter>(GetOwner());
	USceneComponent* AttachedComponent = Cast<USceneComponent>(Player->GetMesh());

	ShieldEffectComponent = UGameplayStatics::SpawnEmitterAttached(ShieldEffect, AttachedComponent);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivateSoundFX, AttachedComponent->GetComponentLocation(), 1.1f);

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
	if (PlayerController)
	{
		PlayerController->ClientPlayCameraShake(ShieldAbilityShake);
	}
}

void UAbilityShield::OnRep_ShieldActive()
{
	if (IsShieldActive == true)
	{
		ActivateAbilityEffects();
	}
	else
	{
		ShieldEffectComponent->DestroyComponent();
	}
}

void UAbilityShield::OnShieldTimerExpired()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		IsShieldActive = false;
		if (ShieldEffectComponent)
		{
			ShieldEffectComponent->DestroyComponent();
		}
	}
}

void UAbilityShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityShield, IsShieldActive);
}
