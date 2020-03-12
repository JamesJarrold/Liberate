// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "World/RegionInfo.h"
#include "GameFramework/Character.h"
#include "Sound/SoundBase.h"
#include "EnemyBase.generated.h"

//Delegate for controlling behaviour after an enemy death
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDeath, AActor*, EnemyActor);

UCLASS()
class LIBERATE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();
	void Initialise(E_REGION_DIFFICULTY difficulty, FBoxSphereBounds& bounds);

	void ReceiveDamage(const float damage);

	UFUNCTION(BlueprintCallable)
		virtual void Attack();

	UFUNCTION(BlueprintCallable)
		virtual void Die();

	UPROPERTY(BlueprintAssignable)
		FEnemyDeath OnEnemyDeath;

protected:	
	UFUNCTION(BlueprintCallable)
		FVector GetRandomPatrolLocation();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Stats)
		float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
		float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
		float Accuracy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
		float AttackRatePerSec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AttackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* HitAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundBase* AttackSoundFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundBase* DeathSoundFX;

	UPROPERTY()
		FBoxSphereBounds PatrolBounds;

	FVector TargetLocation;

	UPROPERTY(ReplicatedUsing = OnRep_IsAttackingAnimation)
		bool IsAttackingAnimation;
	UFUNCTION()
		void OnRep_IsAttackingAnimation();

	UPROPERTY(ReplicatedUsing = OnRep_IsHit)
		bool IsHit;
	UFUNCTION()
		void OnRep_IsHit();

	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
		bool IsDead;
	UFUNCTION()
		void OnRep_IsDead();

	UPROPERTY(ReplicatedUsing = OnRep_HasRagdoll)
		bool HasRagdoll;
	UFUNCTION()
		void OnRep_HasRagdoll();
};
