// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "NavigationSystem.h"
#include "Math/UnrealMathUtility.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UnrealNetwork.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 70.f;
	Damage = 10.f;
	Accuracy = 1.0f;
	AttackRatePerSec = 0.75f;

	this->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	SetReplicates(true);
}

void AEnemyBase::Initialise(E_REGION_DIFFICULTY difficulty, FBoxSphereBounds& bounds)
{
	if (Role == ROLE_Authority)
	{
		const FEnemyStats* EnemyStats = EnemyDifficultyStats.Find(difficulty);
		Health = EnemyStats->Health;
		Damage = EnemyStats->Damage;
		Accuracy = EnemyStats->Accuracy;
		AttackRatePerSec = EnemyStats->AttackRatePerSec;

		PatrolBounds = bounds;

		UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetAIController(this)->GetBlackboardComponent();

		float TimeToAttack = 1.0 / AttackRatePerSec;
		Blackboard->SetValueAsFloat("TimeToAttack", TimeToAttack);
	}
}

FVector AEnemyBase::GetRandomPatrolLocation()
{
	FVector PatrolLocation(0.f, 0.f, 0.f);
	FNavLocation NavLocation;
	bool LocationFound = false;

	while (!LocationFound)
	{
		PatrolLocation.X = FMath::RandRange((PatrolBounds.Origin.X - PatrolBounds.BoxExtent.X), (PatrolBounds.Origin.X + PatrolBounds.BoxExtent.X));
		PatrolLocation.Y = FMath::RandRange((PatrolBounds.Origin.Y - PatrolBounds.BoxExtent.Y), (PatrolBounds.Origin.Y + PatrolBounds.BoxExtent.Y));
		PatrolLocation.Z = FMath::RandRange((PatrolBounds.Origin.Z - PatrolBounds.BoxExtent.Z), (PatrolBounds.Origin.Z + PatrolBounds.BoxExtent.Z));

		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		LocationFound = NavSystem->ProjectPointToNavigation(PatrolLocation, NavLocation, FVector::ZeroVector);

		if (LocationFound)
		{
			TargetLocation = NavLocation.Location;
			return TargetLocation;
		}
	}

	TargetLocation = NavLocation.Location;
	return TargetLocation;
}

void AEnemyBase::ReceiveDamage(const float damage)
{
	if (Role == ROLE_Authority)
	{
		Health -= damage;

		UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetAIController(this)->GetBlackboardComponent();

		if (Health <= 0 && !Blackboard->GetValueAsBool("IsDead"))
		{
			IsDead = !IsDead;
			PlayAnimMontage(DeathAnimation);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSoundFX, GetActorLocation());
			Blackboard->SetValueAsBool("IsDead", true);
		}
		else if (Health > 0)
		{
			IsHit = !IsHit;
			PlayAnimMontage(HitAnimation);
		}
	}
}

void AEnemyBase::OnRep_IsHit()
{
	PlayAnimMontage(HitAnimation);
}

void AEnemyBase::OnRep_IsDead()
{
	PlayAnimMontage(DeathAnimation);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSoundFX, GetActorLocation());
}

void AEnemyBase::Die()
{
	if (Role == ROLE_Authority)
	{
		HasRagdoll = !HasRagdoll;

		OnEnemyDeath.Broadcast(this);

		/* Disable all collision on capsule */
		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

		//Ragdoll
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->StopMovementImmediately();

		SetLifeSpan(20.0f);
	}	
}

void AEnemyBase::OnRep_HasRagdoll()
{
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AEnemyBase::Attack()
{
	if (Role == ROLE_Authority)
	{
		IsAttackingAnimation = !IsAttackingAnimation;
		PlayAnimMontage(AttackAnimation);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSoundFX, GetActorLocation(), 0.6f);
	}
}

void AEnemyBase::OnRep_IsAttackingAnimation()
{
	PlayAnimMontage(AttackAnimation);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSoundFX, GetActorLocation(), 0.6f);
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, Health);
	DOREPLIFETIME(AEnemyBase, IsAttackingAnimation);
	DOREPLIFETIME(AEnemyBase, IsHit);
	DOREPLIFETIME(AEnemyBase, IsDead);
	DOREPLIFETIME(AEnemyBase, HasRagdoll);
}
