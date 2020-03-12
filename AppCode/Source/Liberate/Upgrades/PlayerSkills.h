#pragma once

#include "CoreMinimal.h"
#include "PlayerSkills.generated.h"

UENUM(BlueprintType)
enum UpgradeType
{
	E_HealthUpgrade = 0 UMETA(DisplayName = "Health Upgrade"),
	E_HealthRecovery	UMETA(DisplayName = "Health Recovery"),
	E_Defence			UMETA(DisplayName = "Defence"),
	E_EnergyUpgrade		UMETA(DisplayName = "Energy Upgrade"),
	E_EnergyRecovery	UMETA(DisplayName = "Energy Recovery"),
	E_ShieldAbility		UMETA(DisplayName = "Shield Ability"),
	E_AttackUpgrade		UMETA(DisplayName = "Attack Upgrade"),
	E_BombAbility		UMETA(DisplayName = "Bomb Ability"),
	E_BlastAbility		UMETA(DisplayName = "Blast Ability"),
};

static TMap<int, float> HealthUpgradeValues =
{
	{0, 60.f},
	{1, 70.f},
	{2, 80.f},
	{3, 90.f},
	{4, 100.f},
};

static TMap<int, float> HealthRecoveryValues =
{
	{0, 1.0f},
	{1, 2.0f},
	{2, 3.0f},
	{3, 4.0f},
};

static TMap<int, float> DefenceUpgradeValues =
{
	{0, 0.0f},
	{1, 0.2f},
	{2, 0.4f},
	{3, 0.65f},
	{4, 1.0f},
};

static TMap<int, float> EnergyUpgradeValues =
{
	{0, 60.f},
	{1, 70.f},
	{2, 80.f},
	{3, 90.f},
	{4, 100.f},
};

static TMap<int, float> EnergyRecoveryValues =
{
	{0, 2.0f},
	{1, 3.0f},
	{2, 4.0f},
	{3, 5.0f},
};

static TMap<int, float> ShieldAbilityValues =
{
	{0, 0.0f},
	{1, 5.0f},
	{2, 8.0f},
	{3, 12.0f},
};

static TMap<int, float> AttackUpgradeValues =
{
	{0, 0.6f},
	{1, 0.75f},
	{2, 1.0f},
};
