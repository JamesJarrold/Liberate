// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBasicController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Character.h"

AAIBasicController::AAIBasicController()
{
	AIBlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

	SetGenericTeamId(FGenericTeamId(1));

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	if (AIPerception && SightConfig && HearingConfig)
	{
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->ConfigureSense(*HearingConfig);
		AIPerception->ConfigureSense(*DamageConfig);
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

		SightConfig->SightRadius = 3800.f;
		SightConfig->LoseSightRadius = 3200.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;

		HearingConfig->HearingRange = 2500.f;

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	}

	SetReplicates(true);
}

void AAIBasicController::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(AIBlackboard) || !ensure(AIBehaviourTree))
	{
		return;
	}
	UseBlackboard(AIBlackboard, AIBlackboardComponent);
	RunBehaviorTree(AIBehaviourTree);

	AIBlackboardComponent->SetValueAsVector("TargetLocation", FVector(0, 0, 0));

	if (!ensure(AIPerception))
	{
		return;
	}

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AAIBasicController::CheckSenses);
}

//void AAIBasicController::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//}

void AAIBasicController::CheckSenses(AActor* UpdatedActor, FAIStimulus Stimulus)
{	
	ACharacter* Player = Cast<ACharacter>(UpdatedActor);
	if (Player)
	{
		AIBlackboardComponent->SetValueAsBool("HasSeenPlayer", true);
		AIBlackboardComponent->SetValueAsObject("TargetActor", UpdatedActor);
	}
}
