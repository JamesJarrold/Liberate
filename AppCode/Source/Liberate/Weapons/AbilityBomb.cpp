// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBomb.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UAbilityBomb::UAbilityBomb()
{
	EnergyConsumption = 15.f;
	Damage = 60.f;

	SetIsReplicated(true);
}

void UAbilityBomb::ActivateAbility(USceneComponent* Mesh, const FVector& Position, FVector& ThrowDirection, FVector& Velocity, float DamageModifier)
{
	UGameplayStatics::SpawnEmitterAttached(SpawnEffect, Mesh, FName("Muzzle_02"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivateSoundFX, Position, 0.5f);

	ThrowBomb(Position, ThrowDirection, Velocity, DamageModifier);

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
	if (PlayerController)
	{
		PlayerController->ClientPlayCameraShake(BombAbilityShake);
	}
}

void UAbilityBomb::ThrowBomb(const FVector& Position, FVector& ThrowDirection, FVector& Velocity, const float DamageModifier)
{
	AAbilityBombActor* Bomb = GetWorld()->SpawnActor<AAbilityBombActor>(BombProjectileClass, Position, FRotator::ZeroRotator);

	float FinalDamage = Damage * DamageModifier;
	Velocity.Normalize();
	ThrowDirection += (Velocity / 10);
	ThrowDirection.Z += 0.6f;
	Bomb->ThrowInDirection(GetOwner(), ThrowDirection, FinalDamage);
}
