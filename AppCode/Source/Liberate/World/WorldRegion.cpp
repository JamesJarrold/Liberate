// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldRegion.h"
#include "LiberateGameStateBase.h"
#include "NavigationSystem.h"
#include "Math/UnrealMathUtility.h"
#include "LiberateCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UnrealNetwork.h"

// Sets default values
AWorldRegion::AWorldRegion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RegionOverlay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RegionOverlay"));
	RegionOverlay->SetupAttachment(RootComponent);
	
	SelectedInWorldMap = false;
	ColourPulseDirection = true;
	CurrentOpacity = 0.f;
	
	FRegionInfo regionInfo;
	RegionData = regionInfo;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AWorldRegion::BeginPlay()
{
	Super::BeginPlay();
	RegionOverlay->RegisterComponent();

	InitialiseRegion();
}

void AWorldRegion::InitialiseRegion()
{
	SpawnEnemies();
	RegionData.EnemiesLeft = RegionData.TotalEnemies;

	OnActorBeginOverlap.AddDynamic(this, &AWorldRegion::OnPlayerEnterRegion);

	UpdateOverlayColour();
}

void AWorldRegion::OnPlayerEnterRegion(class AActor* OverlappedActor, class AActor* OtherActor)
{
	ALiberateCharacter* Player = Cast<ALiberateCharacter>(OtherActor);
	if (Player != nullptr)
	{
		Player->OnEnterRegion(RegionData);
	}
}

void AWorldRegion::SpawnEnemies()
{
	if (Role == ROLE_Authority)
	{
		for (int i = 1; i <= RegionData.TotalEnemies; i++)
		{
			bool SpawnLocationFound = false;
			while (!SpawnLocationFound)
			{
				FVector SpawnLocation = GetRandomSpawnLocation();

				FNavLocation NavLocation;
				UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
				if (NavSystem->ProjectPointToNavigation(SpawnLocation, NavLocation, FVector::ZeroVector))
				{
					SpawnLocation = NavLocation.Location;
					float RandYaw = FMath::RandRange(0.f, 360.f);
					FRotator SpawnRotation(0.f, RandYaw, 0.f);

					//Select a random enemy type if more than one type is spawned in this region
					int EnemyTypeIndex = 0;
					if (EnemyTypes.Num() > 1)
					{
						EnemyTypeIndex = FMath::RandRange(0, EnemyTypes.Num() - 1);
					}

					AEnemyBase* enemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyTypes[EnemyTypeIndex], SpawnLocation, SpawnRotation);
					if (enemy != nullptr)
					{
						SpawnLocationFound = true;
						FVector BoundsOrigin;
						FVector BoundsExtent;
						GetActorBounds(true, BoundsOrigin, BoundsExtent);
						FBoxSphereBounds Bounds(BoundsOrigin, BoundsExtent, 0.f);
						enemy->Initialise(RegionData.RegionDifficulty, Bounds);
						enemy->OnEnemyDeath.AddDynamic(this, &AWorldRegion::EnemyDeath);
						Enemies.Push(enemy);
					}
				}
			}
		}
	}
}

FVector AWorldRegion::GetRandomSpawnLocation()
{
	FVector SpawnLocation(0.f, 0.f, 0.f);

	FVector BoundsOrigin;
	FVector BoundsExtent;
	GetActorBounds(true, BoundsOrigin, BoundsExtent);

	SpawnLocation.X = FMath::RandRange((BoundsOrigin.X - BoundsExtent.X), (BoundsOrigin.X + BoundsExtent.X));
	SpawnLocation.Y = FMath::RandRange((BoundsOrigin.Y - BoundsExtent.Y), (BoundsOrigin.Y + BoundsExtent.Y));
	SpawnLocation.Z = FMath::RandRange((BoundsOrigin.Z - BoundsExtent.Z), (BoundsOrigin.Z + BoundsExtent.Z));

	return SpawnLocation;
}

// Called every frame
void AWorldRegion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMapPulseEffect();
}

void AWorldRegion::UpdateMapPulseEffect()
{
	if (SelectedInWorldMap)
	{
		UMaterialInstanceDynamic* DynamicMatInstance = RegionOverlay->CreateAndSetMaterialInstanceDynamic(0);
		FLinearColor MatColour;
		float Opactiy = 0.f;

		if (CurrentOpacity >= 1.0f && ColourPulseDirection)
		{
			ColourPulseDirection = false;
			CurrentOpacity = 1.0f;
		}
		if (CurrentOpacity <= 0.55f && !ColourPulseDirection)
		{
			ColourPulseDirection = true;
			CurrentOpacity = 0.55f;
		}
		if (ColourPulseDirection)
		{
			CurrentOpacity += 0.01f;
		}
		else
		{
			CurrentOpacity -= 0.01f;
		}

		MatColour.R = 1.f - (RegionData.PercentageCompleted / 100.f);
		MatColour.G = (RegionData.PercentageCompleted / 100.f);
		MatColour.B = 0.f;
		MatColour.A = CurrentOpacity;

		if (DynamicMatInstance)
		{
			DynamicMatInstance->SetVectorParameterValue(FName("Color"), MatColour);
			DynamicMatInstance->SetScalarParameterValue(FName("Opacity"), MatColour.A);
		}
	}
	else
	{
		UpdateOverlayColour();
	}
}

void AWorldRegion::UpdateOverlayColour()
{
	UMaterialInstanceDynamic* DynamicMatInstance = RegionOverlay->CreateAndSetMaterialInstanceDynamic(0);

	FLinearColor MatColour;
	MatColour.R = 1.f - (RegionData.PercentageCompleted / 100.f);
	MatColour.G = (RegionData.PercentageCompleted / 100.f);
	MatColour.B = 0.f;
	MatColour.A = 0.55f;

	CurrentOpacity = MatColour.A;

	if (DynamicMatInstance)
	{
		DynamicMatInstance->SetVectorParameterValue(FName("Color"), MatColour);
		DynamicMatInstance->SetScalarParameterValue(FName("Opacity"), MatColour.A);
	}
}

void AWorldRegion::EnemyDeath(AActor* Act)
{
	if (Role == ROLE_Authority)
	{
		RegionData.EnemiesLeft -= 1;
		RegionData.PercentageCompleted = (1.f - ((float)(RegionData.EnemiesLeft) / (float)(RegionData.TotalEnemies))) * 100.f;

		if (RegionData.PercentageCompleted >= 100)
		{
			OnRegionCleared.Broadcast(RegionData);

			int numPlayers = Cast<ALiberateGameStateBase>(UGameplayStatics::GetGameState(GetWorld()))->PlayerArray.Num();
			for (int i = 0; i < numPlayers; i++)
			{
				ALiberateCharacter* Player = Cast<ALiberateCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), i));
				if (Player != nullptr)
				{
					Player->OnRegionCleared(RegionData);
					Player->AwardSkillPoints(RegionData.SkillPointsValue);
				}
			}
		}
	}
}

void AWorldRegion::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWorldRegion, RegionData);
}
