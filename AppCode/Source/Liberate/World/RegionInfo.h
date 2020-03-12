#pragma once

#include "CoreMinimal.h"
#include "RegionInfo.generated.h"

UENUM()
enum E_REGION_DIFFICULTY
{
	E_DIFFICULTY_VERYEASY = 0	UMETA(DisplayName = "Very Easy"),
	E_DIFFICULTY_EASY			UMETA(DisplayName = "Easy"),
	E_DIFFICULTY_MEDIUM			UMETA(DisplayName = "Medium"),
	E_DIFFICULTY_HARD			UMETA(DisplayName = "Hard"),
	E_DIFFICULTY_VERYHARD		UMETA(DisplayName = "Very Hard"),
	E_DIFFICULTY_MAX			UMETA(DisplayName = "Max")
};

USTRUCT(BlueprintType)
struct FEnemyStats
{
	GENERATED_BODY()

	FEnemyStats() {}
	FEnemyStats(float HealthVal, float DamageVal, float AccuracyVal, float AttackRateVal)
	{
		Health = HealthVal;
		Damage = DamageVal;
		Accuracy = AccuracyVal;
		AttackRatePerSec = AttackRateVal;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Accuracy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float AttackRatePerSec;
};

const TMap<E_REGION_DIFFICULTY, FEnemyStats> EnemyDifficultyStats = 
{
	///EnemyStats: Health, Damage, Accuracy, Rate of Fire
	{ E_DIFFICULTY_VERYEASY, FEnemyStats(60.f, 10.f, 1.f, 0.75f) },
	{ E_DIFFICULTY_EASY, FEnemyStats(70.f, 12.f, 1.f, 0.75f) },
	{ E_DIFFICULTY_MEDIUM, FEnemyStats(80.f, 12.f, 0.6f, 1.f) },
	{ E_DIFFICULTY_HARD, FEnemyStats(100.f, 7.f, 0.65f, 2.f) },
	{ E_DIFFICULTY_VERYHARD, FEnemyStats(120.f, 20.f, 0.85f, 1.f) },
};

USTRUCT(BlueprintType)
struct FRegionInfo
{
	GENERATED_BODY()

	FRegionInfo()
	{
		RegionName = "Region";
		RegionDifficulty = E_DIFFICULTY_VERYEASY;
		TotalEnemies = 0;
		EnemiesLeft = TotalEnemies;
		PercentageCompleted = 0.f;
		SkillPointsValue = 0;
	}

	FRegionInfo(FString Name, TEnumAsByte<E_REGION_DIFFICULTY> Difficulty, int EnemiesCount, int SkillPoints)
	{
		RegionName = Name;
		RegionDifficulty = Difficulty;
		TotalEnemies = EnemiesCount;
		EnemiesLeft = TotalEnemies;
		PercentageCompleted = 0.f;
		SkillPointsValue = SkillPoints;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString RegionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TEnumAsByte<E_REGION_DIFFICULTY> RegionDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int TotalEnemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int EnemiesLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float PercentageCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int SkillPointsValue;
};
