// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityBlast.generated.h"

UCLASS(Blueprintable, EditInlineNew, BlueprintType)
class LIBERATE_API UAbilityBlast : public UAbilityBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityBlast();

	UFUNCTION(BlueprintCallable)
		void PreActivateAbility();

	UFUNCTION()
		void ActivateAbility(const FVector& Position, const float DamageModifier);

	UFUNCTION()
		void PlaySpawnEffects();

protected:
	UPROPERTY(EditAnywhere)
		float BlastRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
		TSubclassOf<UCameraShake> BlastAbilityShake;

	UPROPERTY(ReplicatedUsing=OnRep_BlastActive)
		bool IsBlastActive;
	UFUNCTION()
		void OnRep_BlastActive();
};
