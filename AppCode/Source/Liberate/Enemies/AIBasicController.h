// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIBasicController.generated.h"

UCLASS()
class LIBERATE_API AAIBasicController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIBasicController();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = AI)
		UBlackboardData* AIBlackboard;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		UBehaviorTree* AIBehaviourTree;

	UPROPERTY()
		UBlackboardComponent* AIBlackboardComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI)
		UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = AI)
		UAISenseConfig_Damage* DamageConfig;

	UFUNCTION()
		void CheckSenses(AActor* UpdatedActor, FAIStimulus Stimulus);
};
