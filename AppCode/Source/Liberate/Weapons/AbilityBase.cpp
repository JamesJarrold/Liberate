// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBase.h"

UAbilityBase::UAbilityBase()
{
	SpawnEffect = CreateDefaultSubobject<UParticleSystem>(TEXT("SpawnEffect"));
}
