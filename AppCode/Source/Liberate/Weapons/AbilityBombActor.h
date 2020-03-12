// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundBase.h"
#include "AbilityBombActor.generated.h"

UCLASS()
class LIBERATE_API AAbilityBombActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbilityBombActor();

	UFUNCTION()
		void ThrowInDirection(AActor* OwningPlayer, const FVector& FireDirection, const float FinalDamage);

protected:
	UPROPERTY()
		AActor* PlayerOwner;

	UPROPERTY(VisibleAnywhere)
		USphereComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* SpawnParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* BombExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundBase* ExplosionSoundFX;

	UPROPERTY(EditDefaultsOnly)
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(BlueprintReadOnly)
		float Damage;

	UPROPERTY(BlueprintReadOnly)
		float BlastRadius;

	UFUNCTION()
		void Explode(AActor* MyOverlappedActor, AActor* OtherActor);

	UFUNCTION(Server, Reliable)
		void ServerExplode(AActor* MyOverlappedActorm, AActor* OtherActor);

	UFUNCTION()
		void ActivateExplosionEffects();

	UPROPERTY(ReplicatedUsing = OnRep_Exploded)
		bool HasExploded;

	UFUNCTION()
		void OnRep_Exploded();

	UFUNCTION()
		void DestroyBomb();

	UFUNCTION(Server, Reliable)
		void ServerDestroyBomb();
};
