// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBombActor.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/EnemyBase.h"
#include "LiberateCharacter.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "UnrealNetwork.h"

// Sets default values
AAbilityBombActor::AAbilityBombActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlastRadius = 350.f;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(50.f);
	RootComponent = CollisionComponent;

	SpawnParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("SpawnParticle"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);

	HasExploded = false;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AAbilityBombActor::ThrowInDirection(AActor* OwningPlayer, const FVector& FireDirection, const float FinalDamage)
{
	UGameplayStatics::SpawnEmitterAttached(SpawnParticle, Mesh);

	PlayerOwner = OwningPlayer;
	Damage = FinalDamage;

	ProjectileMovementComponent->Velocity = FireDirection * ProjectileMovementComponent->InitialSpeed;
	OnActorBeginOverlap.AddDynamic(this, &AAbilityBombActor::Explode);
}

void AAbilityBombActor::Explode(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (Role < ROLE_Authority)
	{
		ServerExplode(MyOverlappedActor, OtherActor);
	}

	ALiberateCharacter* IsPlayer = Cast<ALiberateCharacter>(OtherActor);
	if (IsPlayer || OtherActor == this || HasExploded)
	{
		return;
	}

	FVector Position = GetActorLocation();
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), Position, 1.0f, PlayerOwner, 1.0f);

	TArray<FHitResult> ActorsHit;
	TArray<AEnemyBase*> EnemiesHit;

	FVector EndPos = Position + (0, 0, -1);

	FCollisionShape AbilitySphere = FCollisionShape::MakeSphere(BlastRadius);
	GetWorld()->SweepMultiByChannel(ActorsHit, Position, EndPos, FQuat::Identity, ECC_WorldStatic, AbilitySphere);

	for (auto& Hit : ActorsHit)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(Hit.GetActor());
		if (Enemy)
		{
			EnemiesHit.AddUnique(Enemy);
		}
	}

	for (auto& Enemy : EnemiesHit)
	{
		float DistanceFromEnemy = abs(FVector::Dist(Enemy->GetActorLocation(), Position) - 50.f);
		float DamagePercentage = 100.f - (((DistanceFromEnemy / BlastRadius) * 100) / 3);
		float FinalDamage = Damage * (DamagePercentage / 100);

		Enemy->ReceiveDamage(FinalDamage);
		UAISense_Damage::ReportDamageEvent(GetWorld(), Enemy, PlayerOwner, 0.f, PlayerOwner->GetActorLocation(), Enemy->GetActorLocation());
	}

	ActivateExplosionEffects();
	HasExploded = true;
}

void AAbilityBombActor::ServerExplode_Implementation(AActor* MyOverlappedActor, AActor* OtherActor)
{
	Explode(MyOverlappedActor, OtherActor);
}

void AAbilityBombActor::OnRep_Exploded()
{
	if (HasExploded)
	{
		ActivateExplosionEffects();
		Destroy();
	}
}

void AAbilityBombActor::ActivateExplosionEffects()
{
	FVector Position = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BombExplosionEffect, Position, FRotator::ZeroRotator, true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSoundFX, Position, 1.4f);
}

void AAbilityBombActor::DestroyBomb()
{
	if (Role < ROLE_Authority)
	{
		ServerDestroyBomb();
	}

	Destroy();
}

void AAbilityBombActor::ServerDestroyBomb_Implementation()
{
	DestroyBomb();
}

void AAbilityBombActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAbilityBombActor, HasExploded);
}