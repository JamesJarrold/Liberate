// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBombActor.h"
#include "AbilityBase.h"
#include "AbilityBomb.generated.h"


UCLASS(Blueprintable, EditInlineNew, BlueprintType)
class LIBERATE_API UAbilityBomb : public UAbilityBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityBomb();

	UFUNCTION()
		void ActivateAbility(USceneComponent* Mesh, const FVector& Position, FVector& ThrowDirection, FVector& Velocity, const float DamageModifier);

	UFUNCTION()
		void ThrowBomb(const FVector& Position, FVector& ThrowDirection, FVector& Velocity, const float DamageModifier);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AAbilityBombActor> BombProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
		TSubclassOf<UCameraShake> BombAbilityShake;
};
