// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "AbilityShield.generated.h"


UCLASS(Blueprintable, EditInlineNew, BlueprintType)
class LIBERATE_API UAbilityShield : public UAbilityBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityShield();

	UFUNCTION()
		virtual void ActivateAbility();

	UFUNCTION()
		void ActivateAbilityEffects();

	FORCEINLINE bool GetShieldIsActive() const { return IsShieldActive; }
	FORCEINLINE void SetShieldStrength(float Strength) { ShieldStrength = Strength; }

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldActive)
		bool IsShieldActive;

	UFUNCTION()
		void OnRep_ShieldActive();

	UFUNCTION()
		void OnShieldTimerExpired();

	UPROPERTY(BlueprintReadOnly)
		float ShieldStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UParticleSystem* ShieldEffect;

	UPROPERTY(BlueprintReadOnly)
		UParticleSystemComponent* ShieldEffectComponent;

	UPROPERTY()
	FTimerHandle ShieldActiveTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
		TSubclassOf<UCameraShake> ShieldAbilityShake;
};
