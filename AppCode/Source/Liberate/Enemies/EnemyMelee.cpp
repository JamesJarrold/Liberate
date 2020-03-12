// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMelee.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LiberateCharacter.h"
#include "UnrealNetwork.h"

AEnemyMelee::AEnemyMelee()
{
	HitParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("HitParticle"));

	SetReplicates(true);
}

void AEnemyMelee::Attack()
{
	Super::Attack();
}

void AEnemyMelee::HitPlayer()
{
	if (Role == ROLE_Authority)
	{
		UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetAIController(this)->GetBlackboardComponent();
		ALiberateCharacter* Character = Cast<ALiberateCharacter>(Blackboard->GetValueAsObject("TargetActor"));

		if (Character != nullptr)
		{
			FVector PlayerPos = Character->GetActorLocation();
			float DistToPlayer = FVector::Dist(PlayerPos, GetActorLocation());

			if (DistToPlayer < 250.f)
			{
				HasHitPlayer = !HasHitPlayer;
				HitLocation = Character->GetActorLocation();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation, FRotator::ZeroRotator, true);
				Character->ReceiveDamage(Damage);
			}
		}
	}
}

void AEnemyMelee::OnRep_HasHitPlayer()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation, FRotator::ZeroRotator, true);
}

void AEnemyMelee::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyMelee, HitLocation);
	DOREPLIFETIME(AEnemyMelee, HasHitPlayer);
}
