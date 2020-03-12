// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapons/PrimaryWeapon.h"
#include "Weapons/AbilityShield.h"
#include "Weapons/AbilityBlast.h"
#include "Weapons/AbilityBomb.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "World/RegionInfo.h"
#include "World/WorldMap.h"
#include "Sound/SoundBase.h"
#include "Camera/PlayerCamera.h"
#include "LiberateCharacter.generated.h"

UCLASS(config=Game)
class ALiberateCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ALiberateCharacter();

	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class ULiberatePlayerCamera* GetFollowCamera() const { return PlayerCamera; }

	void ReceiveDamage(const float& Damage);

	UFUNCTION(BlueprintCallable)
	void PlayerDead();

	UFUNCTION()
		void OnEnterRegion(FRegionInfo& RegionData);

	UFUNCTION()
		void OnRegionCleared(FRegionInfo& RegionData);

	UFUNCTION(BlueprintCallable)
		void AwardSkillPoints(int SkillPointsGained);

	UFUNCTION(BlueprintCallable)
		void PrimaryFire();

	UFUNCTION(BlueprintCallable)
		void ActivateShieldAbility();

	UFUNCTION(BlueprintCallable)
		void ActivateBombAbility();

	UFUNCTION(BlueprintCallable)
		void ActivateBlastAbility();

	UFUNCTION(BlueprintCallable)
		void SpawnAbilityBlastEffect();

	UFUNCTION(BlueprintCallable)
		bool SpendSkillPoints(int SkillPointsCost);

	UFUNCTION()
		void SetShieldStrength(float Strength) { AbilityShield->SetShieldStrength(Strength); }


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseLookUpRate;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float Health;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float HealthMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float HealthRegenPerSec;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float Energy;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float EnergyMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float EnergyRegenPerSec;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float DamageModifier;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = PlayerStats)
		float Defence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Weapons)
		UPrimaryWeapon* PrimaryWeaponBolt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Weapons)
		UAbilityShield* AbilityShield;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Weapons)
		UAbilityBlast* AbilityBlast;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Weapons)
		UAbilityBomb* AbilityBomb;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons)
		bool IsAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* PrimaryAttackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AbilityShieldAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AbilityBlastAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* AbilityBombAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundBase* DeathSoundFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio)
		USoundBase* NoEnergySoundFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UWorldMap* WorldMap;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> PlayerStatsUI;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> RegionPopupUI;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> RegionCompletedUI;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> WinGameScreenUI;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> LoseGameScreenUI;

	UPROPERTY(VisibleAnywhere, Category = UI)
		class UUserWidget* PlayerStats;

	UPROPERTY(VisibleAnywhere, Category = UI)
		class UUserWidget* RegionPopup;

	UPROPERTY(VisibleAnywhere, Category = UI)
		class UUserWidget* RegionCompleted;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		FRegionInfo CurrentRegion;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class ULiberatePlayerCamera* PlayerCamera;

	//Movement & Look Ccntrol
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	FORCEINLINE void AimDownSights() { IsAiming = true;	}
	FORCEINLINE void AimDownSightsReleased() { IsAiming = false; }

	UFUNCTION()
		void PrePrimaryFire();
	UFUNCTION()
		void PreActivateShieldAbility();
	UFUNCTION()
		void PreActivateBlastAbility();
	UFUNCTION()
		void PreActivateBombAbility();

	UFUNCTION(Server, Reliable)
		void ServerPrePrimaryFire();
	UFUNCTION(Server, Reliable)
		void ServerPreActivateShieldAbility();
	UFUNCTION(Server, Reliable)
		void ServerPreActivateBlastAbility();
	UFUNCTION(Server, Reliable)
		void ServerPreActivateBombAbility();

	UFUNCTION(Server, Reliable)
		void ServerPrimaryFire();
	UFUNCTION(Server, Reliable)
		void ServerActivateShieldAbility();
	UFUNCTION(Server, Reliable)
		void ServerActivateBlastAbility();
	UFUNCTION(Server, Reliable)
		void ServerActivateBombAbility();

	UPROPERTY(ReplicatedUsing = OnRep_PrimaryFireActivated)
		bool PrimaryFiredActivated;
	UFUNCTION()
		void OnRep_PrimaryFireActivated();
	UPROPERTY(ReplicatedUsing = OnRep_ShieldAbilityActivated)
		bool ShieldAbilityActivated;
	UFUNCTION()
		void OnRep_ShieldAbilityActivated();
	UPROPERTY(ReplicatedUsing = OnRep_BlastAbilityActivated)
		bool BlastAbilityActivated;
	UFUNCTION()
		void OnRep_BlastAbilityActivated();
	UPROPERTY(ReplicatedUsing = OnRep_BombAbilityActivated)
		bool BombAbilityActivated;
	UFUNCTION()
		void OnRep_BombAbilityActivated();

	UFUNCTION()
		void PlayFootstepSound();
	UFUNCTION(Server, Reliable)
		void ServerPlayFootstepSound();

	UFUNCTION(Server, Reliable)
		void ServerAwardSkillPoints(int SkillPointsGained);

	UFUNCTION(Server, Reliable)
		void ServerSpendSkillPoints(int SkillPointsCost);

	UPROPERTY(ReplicatedUsing = OnRep_GameOver)
		bool IsGameOver;
	UFUNCTION()
		void OnRep_GameOver();
	UPROPERTY(ReplicatedUsing = OnRep_RegionEntered)
		bool HasEnteredRegion;
	UFUNCTION()
		void OnRep_RegionEntered();
	UPROPERTY(ReplicatedUsing = OnRep_RegionCleared)
		bool RegionCleared;
	UFUNCTION()
		void OnRep_RegionCleared();
	UPROPERTY(ReplicatedUsing = OnRep_GameWon)
		bool IsGameWon;
	UFUNCTION()
		void OnRep_GameWon();

	UPROPERTY(ReplicatedUsing=OnRep_IsDead, VisibleAnywhere, BlueprintReadOnly)
		bool IsDead;
	UFUNCTION()
		void OnRep_IsDead();

	FGenericTeamId TeamId;
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

