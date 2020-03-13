// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Camera/CameraShake.h"
#include "PrimaryWeapon.generated.h"

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
		bool TraceFired;

	UPROPERTY()
		FVector_NetQuantize TraceStart;

	UPROPERTY()
		FVector_NetQuantize TraceEnd;
};


UCLASS( ClassGroup=(Weapons), meta=(BlueprintSpawnableComponent) )
class LIBERATE_API UPrimaryWeapon : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPrimaryWeapon();

	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetEnergyConsumption() const { return EnergyConsumption; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire(const UWorld* const world, const FVector& HandLocation, const FVector& StartLocation, FVector& TargetLocation, const float DamageModifier);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float EnergyConsumption;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* SpawnParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* FireParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USoundBase* FireSoundFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
		TSubclassOf<UCameraShake> WeaponFireShake;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace WeaponHitTrace;
	UFUNCTION()
		void OnRep_HitScanTrace();

	UFUNCTION()
		void PlayWeaponFireEffects(FVector StartLocation, FVector TargetLocation);

	UFUNCTION()
		void PlayImpactEffects(FVector ImpactPoint);
};
