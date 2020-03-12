// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class LIBERATE_API AEnemyProjectile : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyProjectile();

	virtual void Tick(float DeltaTime) override;

	virtual void Attack() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* FireParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UParticleSystem* HitParticle;	

	UPROPERTY()
		float AimOffsetMax;

	UPROPERTY(Replicated)
		FVector TargetLocation;

	UPROPERTY(Replicated)
		FVector HitLocation;

	UFUNCTION()
		void UpdateRotation(float DeltaTime);

	FVector CalculateTargetLocation(ACharacter* Player);

	void AttackEffects(FVector& MuzzleLocation, FVector& BulletTargetLocation);
	void WeaponImpactEffects(FVector& BulletHitLocation);

	UPROPERTY(ReplicatedUsing = OnRep_IsAttacking)
		bool IsAttacking;
	UFUNCTION()
		void OnRep_IsAttacking();
};
