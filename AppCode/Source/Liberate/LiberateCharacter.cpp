// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "LiberateCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Perception/AISense_Hearing.h"
#include "LiberatePlayerState.h"
#include "UnrealNetwork.h"
#include "World/WorldRegion.h"
#include "Math/UnrealMathVectorCommon.h"

ALiberateCharacter::ALiberateCharacter()
{
	TeamId = FGenericTeamId(0);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 175.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	PlayerCamera = CreateDefaultSubobject<ULiberatePlayerCamera>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	PlayerCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	PlayerCamera->RelativeLocation = FVector(0.f, 0.f, 50.f);

	IsAttacking = false;
	IsAiming = false;
	IsDead = false;

	PrimaryWeaponBolt = CreateDefaultSubobject<UPrimaryWeapon>(TEXT("PrimaryWeaponBolt"));
	PrimaryWeaponBolt->SetupAttachment(RootComponent);
	AbilityShield = CreateDefaultSubobject<UAbilityShield>(TEXT("AbilityShield"));
	AbilityBomb = CreateDefaultSubobject<UAbilityBomb>(TEXT("AbilityBomb"));
	AbilityBlast = CreateDefaultSubobject<UAbilityBlast>(TEXT("AbilityBlast"));

	WorldMap = CreateDefaultSubobject<UWorldMap>(TEXT("WorldMap"));
	PlayerStats = CreateDefaultSubobject<UUserWidget>(TEXT("PlayerStats"));
	RegionPopup = CreateDefaultSubobject<UUserWidget>(TEXT("RegionPopup"));
}

void ALiberateCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		WorldMap->InitialiseMap(this);
	}
}

void ALiberateCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	WorldMap->InitialiseMap(this);
}

void ALiberateCharacter::Tick(float DeltaTime)
{
	///Health and Energy Regen
	if (Energy < EnergyMax)
	{
		Energy += EnergyRegenPerSec * DeltaTime;
	}
	if (Health < HealthMax)
	{
		Health += HealthRegenPerSec * DeltaTime;
	}

	///Lerp Between aiming states
	FVector CameraPos = PlayerCamera->RelativeLocation;

	if (IsAiming)
	{
		PlayerCamera->SetRelativeLocation(FMath::Lerp(CameraPos, FVector(100.f, 30.f, 75.f), 0.06f));
	}
	else
	{
		PlayerCamera->SetRelativeLocation(FMath::Lerp(CameraPos, FVector(-25.f, 0.f, 100.f), 0.04f));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALiberateCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &ALiberateCharacter::PrePrimaryFire);
	PlayerInputComponent->BindAction("AbilityShield", IE_Pressed, this, &ALiberateCharacter::PreActivateShieldAbility);
	PlayerInputComponent->BindAction("AbilityBlast", IE_Pressed, this, &ALiberateCharacter::PreActivateBlastAbility);
	PlayerInputComponent->BindAction("AbilityBomb", IE_Pressed, this, &ALiberateCharacter::PreActivateBombAbility);

	PlayerInputComponent->BindAction("AimDownSight", IE_Pressed, this, &ALiberateCharacter::AimDownSights);
	PlayerInputComponent->BindAction("AimDownSight", IE_Released, this, &ALiberateCharacter::AimDownSightsReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALiberateCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALiberateCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALiberateCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALiberateCharacter::LookUpAtRate);
}

void ALiberateCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALiberateCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ALiberateCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		PlayFootstepSound();

		Cast<APlayerController>(GetController())->PlayerCameraManager->PlayCameraShake(PlayerCamera->GetRunningCameraShake());
	}
}

void ALiberateCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);

		PlayFootstepSound();

		Cast<APlayerController>(GetController())->ClientPlayCameraShake(PlayerCamera->GetRunningCameraShake());
	}
}

void ALiberateCharacter::PlayFootstepSound()
{
	if (Role < ROLE_Authority)
	{
		ServerPlayFootstepSound();
		return;
	}

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this, 0.0f);
}

void ALiberateCharacter::ServerPlayFootstepSound_Implementation()
{
	PlayFootstepSound();
}

void ALiberateCharacter::PrePrimaryFire()
{
	if (Role < ROLE_Authority)
	{
		ServerPrePrimaryFire();
		return;
	}

	float energyUsed = PrimaryWeaponBolt->GetEnergyConsumption();

	if ((Energy - energyUsed) >= 0)
	{
		PrimaryFiredActivated = true;
		PlayAnimMontage(PrimaryAttackAnimation, 1.5f);
		Energy -= energyUsed;
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoEnergySoundFX, GetActorLocation());
	}
}

void ALiberateCharacter::PreActivateShieldAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerPreActivateShieldAbility();
		return;
	}

	float EnergyNeeded = AbilityShield->GetEnergyConsumption();
	ALiberatePlayerState* CurrPlayerState = Cast<ALiberatePlayerState>(GetPlayerState());

	if (!(CurrPlayerState->isShieldUnlocked()) || AbilityShield->GetShieldIsActive())
	{
		return;
	}
	else if (EnergyNeeded > Energy)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoEnergySoundFX, GetActorLocation());
		return;
	}

	ShieldAbilityActivated = true;
	Energy -= EnergyNeeded;
	PlayAnimMontage(AbilityShieldAnimation);
}

void ALiberateCharacter::PreActivateBlastAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerPreActivateBlastAbility();
		return;
	}

	float EnergyNeeded = AbilityBlast->GetEnergyConsumption();
	ALiberatePlayerState* CurrPlayerState = Cast<ALiberatePlayerState>(GetPlayerState());

	if (!(CurrPlayerState->isBlastUnlocked()))
	{
		return;
	}
	else if (EnergyNeeded > Energy)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoEnergySoundFX, GetActorLocation());
		return;
	}

	BlastAbilityActivated = true;
	Energy -= EnergyNeeded;

	PlayAnimMontage(AbilityBlastAnimation, 0.75f);
	PlayerCamera->ActivateBlastAbilityAnimation();
}

void ALiberateCharacter::PreActivateBombAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerPreActivateBombAbility();
		return;
	}

	float EnergyNeeded = AbilityBomb->GetEnergyConsumption();
	ALiberatePlayerState* CurrPlayerState = Cast<ALiberatePlayerState>(GetPlayerState());

	if (!(CurrPlayerState->isBombUnlocked()))
	{
		return;
	}
	else if (EnergyNeeded > Energy)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoEnergySoundFX, GetActorLocation());
		return;
	}

	BombAbilityActivated = true;
	Energy -= EnergyNeeded;
	PlayAnimMontage(AbilityBombAnimation, 1.5f);
}

void ALiberateCharacter::ServerPrePrimaryFire_Implementation()
{
	PrePrimaryFire();
}

void ALiberateCharacter::ServerPreActivateShieldAbility_Implementation()
{
	PreActivateShieldAbility();
}

void ALiberateCharacter::ServerPreActivateBlastAbility_Implementation()
{
	PreActivateBlastAbility();
}

void ALiberateCharacter::ServerPreActivateBombAbility_Implementation()
{
	PreActivateBombAbility();
}

void ALiberateCharacter::OnRep_PrimaryFireActivated()
{
	if (PrimaryFiredActivated)
	{
		PlayAnimMontage(PrimaryAttackAnimation, 1.5f);
	}
}

void ALiberateCharacter::OnRep_ShieldAbilityActivated()
{
	if (ShieldAbilityActivated)
	{
		PlayAnimMontage(AbilityShieldAnimation);
	}
}

void ALiberateCharacter::OnRep_BlastAbilityActivated()
{
	if (BlastAbilityActivated)
	{
		PlayerCamera->ActivateBlastAbilityAnimation();
		PlayAnimMontage(AbilityBlastAnimation, 0.75f);
	}
}

void ALiberateCharacter::OnRep_BombAbilityActivated()
{
	if (BombAbilityActivated)
	{
		PlayAnimMontage(AbilityBombAnimation, 1.5f);
	}
}

void ALiberateCharacter::PrimaryFire()
{
	if (Role < ROLE_Authority)
	{
		ServerPrimaryFire();
		return;
	}

	if (PrimaryFiredActivated)
	{
		FVector CameraLocation = PlayerCamera->GetComponentLocation();
		FRotator CameraRotation = PlayerCamera->GetComponentRotation();

		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
		FVector TargetLocation = CameraLocation + (CameraRotation.Vector() * 8000);

		PrimaryWeaponBolt->Fire(GetWorld(), HandLocation, CameraLocation, TargetLocation, DamageModifier);

		PrimaryFiredActivated = false;
	}
}

void ALiberateCharacter::ActivateShieldAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerActivateShieldAbility();
		return;
	}

	if (ShieldAbilityActivated)
	{
		AbilityShield->ActivateAbility();

		ShieldAbilityActivated = false;
	}
}

void ALiberateCharacter::ActivateBlastAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerActivateBlastAbility();
		return;
	}

	if (BlastAbilityActivated)
	{
		AbilityBlast->ActivateAbility(GetActorLocation(), DamageModifier);

		BlastAbilityActivated = false;
	}
}

void ALiberateCharacter::ActivateBombAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerActivateBombAbility();
		return;
	}

	if (BombAbilityActivated)
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_02");
		FVector ForwardVec = GetFollowCamera()->GetForwardVector();
		FVector Velocity = GetVelocity();
		AbilityBomb->ActivateAbility(GetMesh(), HandLocation, ForwardVec, Velocity, DamageModifier);

		BombAbilityActivated = false;
	}
}

void ALiberateCharacter::ServerPrimaryFire_Implementation()
{
	PrimaryFire();
}

void ALiberateCharacter::ServerActivateShieldAbility_Implementation()
{
	ActivateShieldAbility();
}

void ALiberateCharacter::ServerActivateBlastAbility_Implementation()
{
	ActivateBlastAbility();
}

void ALiberateCharacter::ServerActivateBombAbility_Implementation()
{
	ActivateBombAbility();
}

void ALiberateCharacter::SpawnAbilityBlastEffect()
{
	AbilityBlast->PreActivateAbility();
}

void ALiberateCharacter::ReceiveDamage(const float Damage)
{
	if (Role == ROLE_Authority)
	{
		float DamageTaken = 0.f;
		float DefenceLevel = 0.f;

		if (AbilityShield->GetShieldIsActive())
		{
			//Take no damage if shield is active, so return
			return;
		}

		DefenceLevel = (Energy / 1.5f) + ((Defence / 3.0f) * 100);
		DamageTaken = Damage - ((DefenceLevel / 150) * Damage);
		Health -= DamageTaken;

		if (Health <= 0 && !IsDead)
		{
			IsDead = true;
			DisableInput(Cast<APlayerController>(GetController()));
			PlayAnimMontage(DeathAnimation);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSoundFX, GetActorLocation(), 1.1f);
		}
	}
}

void ALiberateCharacter::OnRep_IsDead()
{
	DisableInput(Cast<APlayerController>(GetController()));
	PlayAnimMontage(DeathAnimation);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSoundFX, GetActorLocation(), 1.1f);
}

void ALiberateCharacter::PlayerDead()
{
	IsGameOver = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(GetController()), EQuitPreference::Type::Quit, false);
	
	if (!IsLocallyControlled())
	{
		return;
	}

	///Called by an anim notify at the end of the player death animation montage
	Cast<APlayerController>(GetController())->bShowMouseCursor = true;
	
	UUserWidget* LoseGameScreen = CreateWidget<UUserWidget>(GetWorld(), LoseGameScreenUI);
	if (LoseGameScreen)
	{
		LoseGameScreen->AddToViewport();
	}
}

void ALiberateCharacter::OnRep_GameOver()
{
	if (IsLocallyControlled())
	{
		Cast<APlayerController>(GetController())->bShowMouseCursor = true;

		UUserWidget* LoseGameScreen = CreateWidget<UUserWidget>(GetWorld(), LoseGameScreenUI);
		if (LoseGameScreen)
		{
			LoseGameScreen->AddToViewport();
		}
	}
}

void ALiberateCharacter::OnEnterRegion(FRegionInfo& RegionData)
{
	HasEnteredRegion = !HasEnteredRegion;
	CurrentRegion = RegionData;

	if (!IsLocallyControlled())
	{
		return;
	}

	RegionPopup = CreateWidget<UUserWidget>(GetWorld(), RegionPopupUI);
	if (RegionPopup)
	{
		RegionPopup->AddToViewport();
	}
}

void ALiberateCharacter::OnRep_RegionEntered()
{
	if (IsLocallyControlled())
	{
		RegionPopup = CreateWidget<UUserWidget>(GetWorld(), RegionPopupUI);
		if (RegionPopup)
		{
			RegionPopup->AddToViewport();
		}
	}
}

void ALiberateCharacter::OnRegionCleared(FRegionInfo& RegionData)
{
	RegionCleared = !RegionCleared;
	CurrentRegion = RegionData;

	if (!IsLocallyControlled())
	{
		return;
	}

	RegionCompleted = CreateWidget<UUserWidget>(GetWorld(), RegionCompletedUI);
	if (RegionCompleted)
	{
		RegionCompleted->AddToViewport();
	}
}

void ALiberateCharacter::OnRep_RegionCleared()
{
	if (IsLocallyControlled())
	{
		RegionCompleted = CreateWidget<UUserWidget>(GetWorld(), RegionCompletedUI);
		if (RegionCompleted)
		{
			RegionCompleted->AddToViewport();
		}
	}
}

void ALiberateCharacter::OnRep_GameWon()
{
	if (IsLocallyControlled())
	{
		///TODO: Add win result
	}
}

void ALiberateCharacter::AwardSkillPoints(const int SkillPointsGained)
{
	if (Role < ROLE_Authority)
	{
		ServerAwardSkillPoints(SkillPointsGained);
		return;
	}

	ALiberatePlayerState* CurrPlayerState = Cast<ALiberatePlayerState>(GetPlayerState());
	int SkillPoints = CurrPlayerState->GetSkillPoints();

	SkillPoints += SkillPointsGained;
	CurrPlayerState->SetSkillPoints(SkillPoints);
}

void ALiberateCharacter::ServerAwardSkillPoints_Implementation(const int SkillPointsGained)
{
	AwardSkillPoints(SkillPointsGained);
}

bool ALiberateCharacter::SpendSkillPoints(const int SkillPointsCost)
{
	if (Role < ROLE_Authority)
	{
		ServerSpendSkillPoints(SkillPointsCost);
	}

	ALiberatePlayerState* CurrPlayerState = Cast<ALiberatePlayerState>(GetPlayerState());
	int SkillPoints = CurrPlayerState->GetSkillPoints();

	if (SkillPoints >= SkillPointsCost)
	{
		SkillPoints -= SkillPointsCost;
		CurrPlayerState->SetSkillPoints(SkillPoints);
		return true;
	}
	else
	{
		return false;
	}
}

void ALiberateCharacter::ServerSpendSkillPoints_Implementation(const int SkillPointsCost)
{
	SpendSkillPoints(SkillPointsCost);
}

void ALiberateCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALiberateCharacter, Health); 
	DOREPLIFETIME(ALiberateCharacter, HealthMax);
	DOREPLIFETIME(ALiberateCharacter, Energy);
	DOREPLIFETIME(ALiberateCharacter, EnergyMax);
	DOREPLIFETIME(ALiberateCharacter, DamageModifier);
	DOREPLIFETIME(ALiberateCharacter, Defence);
	DOREPLIFETIME(ALiberateCharacter, IsDead);

	DOREPLIFETIME(ALiberateCharacter, PrimaryWeaponBolt); 
	DOREPLIFETIME(ALiberateCharacter, AbilityShield);
	DOREPLIFETIME(ALiberateCharacter, AbilityBlast);
	DOREPLIFETIME(ALiberateCharacter, AbilityBomb);

	DOREPLIFETIME(ALiberateCharacter, PrimaryFiredActivated);
	DOREPLIFETIME(ALiberateCharacter, ShieldAbilityActivated);
	DOREPLIFETIME(ALiberateCharacter, BlastAbilityActivated);
	DOREPLIFETIME(ALiberateCharacter, BombAbilityActivated);

	DOREPLIFETIME(ALiberateCharacter, CurrentRegion);
	DOREPLIFETIME(ALiberateCharacter, IsGameOver);
	DOREPLIFETIME(ALiberateCharacter, HasEnteredRegion);
	DOREPLIFETIME(ALiberateCharacter, RegionCleared);
	DOREPLIFETIME(ALiberateCharacter, IsGameWon);
}