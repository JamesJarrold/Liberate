// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Upgrades/PlayerSkills.h"
#include "GameFramework/PlayerState.h"
#include "LiberatePlayerState.generated.h"

UCLASS()
class LIBERATE_API ALiberatePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void UpgradeSkill(UpgradeType type, class ALiberateCharacter* Player);

	UFUNCTION(Server, Reliable)
		void ServerUpgradeSkill(UpgradeType type, class ALiberateCharacter* Player);

	UFUNCTION(BlueprintCallable)
		void InitialisePlayerSkills(class ALiberateCharacter* Player);

	FORCEINLINE bool isShieldUnlocked() const { return ShieldAbilityUnlocked; }
	FORCEINLINE bool isBombUnlocked() const { return BombAbilityUnlocked; }
	FORCEINLINE bool isBlastUnlocked() const { return BlastAbilityUnlocked; }

	FORCEINLINE int GetSkillPoints() const { return SkillPoints; }
	FORCEINLINE void SetSkillPoints(int SkillPointCount) { SkillPoints = SkillPointCount;  }

protected:
	UPROPERTY(Replicated, BlueprintReadOnly)
		int SkillPoints = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int HealthUpgradeLevel = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int HealthRecoveryLevel = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int DefenceLevel = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int EnergyUpgradeLevel = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int EnergyRecoveryLevel = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int ShieldAbilityLevel = 0;

	UPROPERTY(Replicated, BlueprintReadWrite)
		int AttackUpgradeLevel = 0;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
		bool ShieldAbilityUnlocked = false;

	UPROPERTY(Replicated, BlueprintReadWrite)
		bool BombAbilityUnlocked = false;

	UPROPERTY(Replicated, BlueprintReadWrite)
		bool BlastAbilityUnlocked = false;
};
