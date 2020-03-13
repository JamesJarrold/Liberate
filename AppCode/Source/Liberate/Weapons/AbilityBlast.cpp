// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBlast.h"
#include "DrawDebugHelpers.h"
#include "WorldCollision.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UAbilityBlast::UAbilityBlast()
{
	EnergyConsumption = 40.f;
	Damage = 100.f;
	BlastRadius = 600.f;

	SetIsReplicated(true);
}

void UAbilityBlast::PreActivateAbility()
{
	PlaySpawnEffects();

	IsBlastActive = true;
}

void UAbilityBlast::OnRep_BlastActive()
{
	if (IsBlastActive)
	{
		PlaySpawnEffects();
	}
}

void UAbilityBlast::ActivateAbility(const FVector& Position, float DamageModifier)
{
	///Blast ability creates an explosion around the player which deals damage to all enemies in the radius and knocks them back

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetOwner()->GetActorLocation(), 1.0f, GetOwner(), 1.0f);
	TArray<FHitResult> EnemiesHit;
	FVector EndPos = Position + (0, 0, -1);
	FCollisionShape AbilitySphere = FCollisionShape::MakeSphere(BlastRadius);

	GetWorld()->SweepMultiByChannel(EnemiesHit, Position, EndPos, FQuat::Identity, ECC_WorldStatic, AbilitySphere);
	
	for (auto& Hit : EnemiesHit)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(Hit.GetActor());
		if (Enemy)
		{
			FVector EnemyPos = Enemy->GetActorLocation();

			FVector ImpulseDirection = EnemyPos - Position;
			ImpulseDirection.Normalize();

			FVector ImpulsePower = ImpulseDirection * 2000.f;
			ImpulsePower.Z /= ImpulsePower.Z;
			ImpulsePower.Z *= 500.f;

			Enemy->LaunchCharacter(ImpulsePower, true, true);

			float FinalDamage = Damage * DamageModifier;
			Enemy->ReceiveDamage(FinalDamage);
			UAISense_Damage::ReportDamageEvent(GetWorld(), Enemy, GetOwner(), 0.f, GetOwner()->GetActorLocation(), Hit.Location);
		}
	}

	IsBlastActive = false;
}

void UAbilityBlast::PlaySpawnEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnEffect, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivateSoundFX, GetOwner()->GetActorLocation(), 1.8f);

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
	if (PlayerController)
	{
		PlayerController->ClientPlayCameraShake(BlastAbilityShake);
	}
}

void UAbilityBlast::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityBlast, IsBlastActive);
}
