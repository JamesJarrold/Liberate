// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "EnemyMelee.generated.h"

UCLASS()
class LIBERATE_API AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyMelee();

	virtual void Attack() override;

	UFUNCTION(BlueprintCallable)
		virtual void HitPlayer();

	UPROPERTY(EditDefaultsOnly)
		UParticleSystem* HitParticle;

protected:
	UPROPERTY(Replicated)
		FVector HitLocation;

	UPROPERTY(ReplicatedUsing = OnRep_HasHitPlayer)
		bool HasHitPlayer;
	UFUNCTION()
		void OnRep_HasHitPlayer();
};
