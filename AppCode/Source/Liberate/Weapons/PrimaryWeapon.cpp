// Fill out your copyright notice in the Description page of Project Settings.

#include "PrimaryWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Enemies/EnemyBase.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "UnrealNetwork.h"

// Sets default values for this component's properties
UPrimaryWeapon::UPrimaryWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Damage = 15.0f;
	EnergyConsumption = 3.5f;

	SpawnParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("SpawnParticle"));
	FireParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("FireParticle"));
	HitParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("HitParticle"));

	SetIsReplicated(true);
}

void UPrimaryWeapon::Fire(const UWorld* const world, const FVector& HandLocation, const FVector& StartLocation, FVector& TargetLocation, const float DamageModifier)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult Hit;
	bool HitDetected = (world->LineTraceSingleByChannel(Hit, StartLocation, TargetLocation, ECC_Visibility, QueryParams));
	if (HitDetected)
	{
		PlayImpactEffects(Hit.Location);

		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Hit.Location, 1.0f, GetOwner(), 1.0f);

		AEnemyBase* Enemy = Cast<AEnemyBase>(Hit.Actor);
		if (Enemy != nullptr)
		{
			float FinalDamage = Damage * DamageModifier;
			Enemy->ReceiveDamage(FinalDamage);
			UAISense_Damage::ReportDamageEvent(GetWorld(), Enemy, GetOwner(), 0.f, GetOwner()->GetActorLocation(), Hit.Location);
		}

		TargetLocation = Hit.Location;
	}

	PlayWeaponFireEffects(HandLocation, TargetLocation);

	WeaponHitTrace.TraceFired = !WeaponHitTrace.TraceFired;
	WeaponHitTrace.TraceStart = HandLocation;
	WeaponHitTrace.TraceEnd = TargetLocation;
}

void UPrimaryWeapon::PlayWeaponFireEffects(FVector StartLocation, FVector TargetLocation)
{
	UParticleSystemComponent* fireParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, StartLocation);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSoundFX, StartLocation, 1.2f);

	fireParticle->SetBeamSourcePoint(0, StartLocation, 0);
	fireParticle->SetBeamTargetPoint(0, TargetLocation, 0);

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
	if (PlayerController)
	{
		PlayerController->ClientPlayCameraShake(WeaponFireShake);
	}
}

void UPrimaryWeapon::PlayImpactEffects(FVector ImpactPoint)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
}

void UPrimaryWeapon::OnRep_HitScanTrace()
{
	PlayWeaponFireEffects(WeaponHitTrace.TraceStart, WeaponHitTrace.TraceEnd);
	PlayImpactEffects(WeaponHitTrace.TraceEnd);
}

void UPrimaryWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPrimaryWeapon, WeaponHitTrace);
}