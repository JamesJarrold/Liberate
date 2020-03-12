// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "AIBasicController.h"
#include "LiberateCharacter.h"
#include "UnrealNetwork.h"

AEnemyProjectile::AEnemyProjectile()
{
	FireParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("FireParticle"));
	HitParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("HitParticle"));

	//Player capsule radius - 1
	AimOffsetMax = 23.f;

	SetReplicates(true);
}

void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotation(DeltaTime);
}

void AEnemyProjectile::UpdateRotation(float DeltaTime)
{
	if (Role == ROLE_Authority)
	{
		UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetAIController(this)->GetBlackboardComponent();
		bool CanSeePlayer = Blackboard->GetValueAsBool("HasSeenPlayer");

		//If the enemy can see the player/target then they should update their rotation to be looking at them
		if (CanSeePlayer)
		{
			UObject* Target = Blackboard->GetValueAsObject("TargetActor");
			ACharacter* Player = Cast<ACharacter>(Target);
			if (Player)
			{
				FVector PlayerLocation = Player->GetActorLocation();

				FRotator RotationValue = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);
				FRotator Rotation = FMath::RInterpTo(GetActorRotation(), RotationValue, DeltaTime, 2.0);
				SetActorRotation(RotationValue);
			}
		}
	}
}

void AEnemyProjectile::Attack()
{
	if (Role == ROLE_Authority)
	{
		Super::Attack();

		IsAttacking = !IsAttacking;

		UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetAIController(this)->GetBlackboardComponent();
		UObject* TargetActor = Blackboard->GetValueAsObject("TargetActor");
		ACharacter* Player = Cast<ACharacter>(TargetActor);

		float FinalAimOffset = AimOffsetMax / Accuracy;
		float OffsetX = FMath::RandRange(0 - FinalAimOffset, FinalAimOffset);
		float OffsetY = FMath::RandRange(0 - FinalAimOffset, FinalAimOffset);

		FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle_01");
		TargetLocation = CalculateTargetLocation(Player);

		AttackEffects(MuzzleLocation, TargetLocation);

		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = true;
		QueryParams.AddIgnoredActor(GetOwner());

		FHitResult Hit;
		bool HitDetected = (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, TargetLocation, ECC_Visibility, QueryParams));

		if (HitDetected)
		{
			HitLocation = Hit.Location;
			WeaponImpactEffects(HitLocation);

			ALiberateCharacter* HitPlayer = Cast<ALiberateCharacter>(Hit.Actor);
			if (HitPlayer != nullptr)
			{
				HitPlayer->ReceiveDamage(Damage);
			}
		}
	}
}

FVector AEnemyProjectile::CalculateTargetLocation(ACharacter* Player)
{
	FVector BulletTargetLocation;
	float FinalAimOffset = AimOffsetMax / Accuracy;
	float OffsetX = FMath::RandRange(0 - FinalAimOffset, FinalAimOffset);
	float OffsetY = FMath::RandRange(0 - FinalAimOffset, FinalAimOffset);
	FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	BulletTargetLocation.X = Player->GetActorLocation().X + OffsetX;
	BulletTargetLocation.Y = Player->GetActorLocation().Y + OffsetY;
	BulletTargetLocation.Z = Player->GetActorLocation().Z + OffsetY;
	BulletTargetLocation -= MuzzleLocation;
	BulletTargetLocation.Normalize();
	BulletTargetLocation *= 5000.f;
	BulletTargetLocation += MuzzleLocation;

	return BulletTargetLocation;
}

void AEnemyProjectile::AttackEffects(FVector& MuzzleLocation, FVector& BulletTargetLocation)
{
	UParticleSystemComponent* fireParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, MuzzleLocation, FRotator::ZeroRotator, true);

	fireParticle->SetBeamSourcePoint(0, MuzzleLocation, 0);
	fireParticle->SetBeamTargetPoint(0, TargetLocation, 0);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation, FRotator::ZeroRotator, true);
}

void AEnemyProjectile::WeaponImpactEffects(FVector& BulletHitLocation)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation, FRotator::ZeroRotator, true);
}

void AEnemyProjectile::OnRep_IsAttacking()
{
	FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	AttackEffects(MuzzleLocation, TargetLocation);
	WeaponImpactEffects(HitLocation);
}

void AEnemyProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyProjectile, TargetLocation);
	DOREPLIFETIME(AEnemyProjectile, HitLocation);
	DOREPLIFETIME(AEnemyProjectile, IsAttacking);
}