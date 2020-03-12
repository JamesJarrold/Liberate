// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilityBase.generated.h"

UCLASS(abstract)
class LIBERATE_API UAbilityBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbilityBase();

	FORCEINLINE float GetEnergyConsumption() const { return EnergyConsumption; }
	FORCEINLINE float GetDamage() const { return Damage; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float EnergyConsumption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* SpawnEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
		USoundBase* ActivateSoundFX;
};
