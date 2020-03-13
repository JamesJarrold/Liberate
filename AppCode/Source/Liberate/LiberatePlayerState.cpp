// Fill out your copyright notice in the Description page of Project Settings.


#include "LiberatePlayerState.h"
#include "LiberateCharacter.h"
#include "GameFramework/Controller.h"
#include "UnrealNetwork.h"

void ALiberatePlayerState::UpgradeSkill(const UpgradeType type, ALiberateCharacter* Player)
{
	if (Role < ROLE_Authority)
	{
		ServerUpgradeSkill(type, Player);
	}

	if (type == E_HealthUpgrade)
	{
		HealthUpgradeLevel++;
		Player->HealthMax = *HealthUpgradeValues.Find(HealthUpgradeLevel);
	}
	else if (type == E_HealthRecovery)
	{
		HealthRecoveryLevel++;
		
		Player->HealthRegenPerSec = *HealthRecoveryValues.Find(HealthRecoveryLevel);
	}
	else if (type == E_Defence)
	{
		DefenceLevel++;
		
		Player->Defence = *DefenceUpgradeValues.Find(DefenceLevel);
	}
	else if (type == E_EnergyUpgrade)
	{
		EnergyUpgradeLevel++;
		
		Player->EnergyMax = *EnergyUpgradeValues.Find(EnergyUpgradeLevel);
	}
	else if (type == E_EnergyRecovery)
	{
		EnergyRecoveryLevel++;
		
		Player->EnergyRegenPerSec = *EnergyRecoveryValues.Find(EnergyRecoveryLevel);
	}
	else if (type == E_ShieldAbility)
	{
		ShieldAbilityUnlocked = true;
		ShieldAbilityLevel++;
		
		Player->SetShieldStrength(*ShieldAbilityValues.Find(ShieldAbilityLevel));
	}
	else if (type == E_AttackUpgrade)
	{
		AttackUpgradeLevel++;
		
		Player->DamageModifier = *AttackUpgradeValues.Find(AttackUpgradeLevel);
	}
	else if (type == E_BombAbility)
	{
		BombAbilityUnlocked = true;
	}
	else if (type == E_BlastAbility)
	{
		BlastAbilityUnlocked = true;
	}
}

void ALiberatePlayerState::ServerUpgradeSkill_Implementation(const UpgradeType type, ALiberateCharacter* Player)
{
	UpgradeSkill(type, Player);
}

void ALiberatePlayerState::InitialisePlayerSkills(ALiberateCharacter* Player)
{
	//Skill Points set to 75 for purpose of the demo
	SkillPoints = 75;

	Player->HealthMax = *HealthUpgradeValues.Find(HealthUpgradeLevel);
	Player->Health = Player->HealthMax;
	Player->HealthRegenPerSec = *HealthRecoveryValues.Find(HealthRecoveryLevel);
	Player->Defence = *DefenceUpgradeValues.Find(DefenceLevel);
	Player->EnergyMax = *EnergyUpgradeValues.Find(EnergyUpgradeLevel);
	Player->Energy = Player->EnergyMax;
	Player->EnergyRegenPerSec = *EnergyRecoveryValues.Find(EnergyRecoveryLevel);
	Player->SetShieldStrength(*ShieldAbilityValues.Find(ShieldAbilityLevel));
	Player->DamageModifier = *AttackUpgradeValues.Find(AttackUpgradeLevel);
}

void ALiberatePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALiberatePlayerState, SkillPoints);
	DOREPLIFETIME(ALiberatePlayerState, HealthUpgradeLevel);
	DOREPLIFETIME(ALiberatePlayerState, HealthRecoveryLevel);
	DOREPLIFETIME(ALiberatePlayerState, DefenceLevel);
	DOREPLIFETIME(ALiberatePlayerState, EnergyUpgradeLevel);
	DOREPLIFETIME(ALiberatePlayerState, EnergyRecoveryLevel);
	DOREPLIFETIME(ALiberatePlayerState, ShieldAbilityLevel);
	DOREPLIFETIME(ALiberatePlayerState, AttackUpgradeLevel);
	DOREPLIFETIME(ALiberatePlayerState, ShieldAbilityUnlocked);
	DOREPLIFETIME(ALiberatePlayerState, BombAbilityUnlocked);
	DOREPLIFETIME(ALiberatePlayerState, BlastAbilityUnlocked);
}