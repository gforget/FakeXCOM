// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "ActorsObject/Gun.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

void AGun::SetRange(float value)
{
	Range = value;
}

float AGun::GetRange()
{
	return Range;
}

void AGun::SetRangeAimValueCurve(FRuntimeFloatCurve value)
{
	RangeAimValueCurve = value;
}

FRuntimeFloatCurve AGun::GetRangeAimValueCurve()
{
	return RangeAimValueCurve;
}

void AGun::SetAmmo(int value)
{
	Ammo = FMath::Clamp(value, 0, MaxAmmo);
	OnGunAmmoChangeEvent.Broadcast(this);
}

int AGun::GetAmmo()
{
	return FMath::Clamp(Ammo, 0, MaxAmmo);
}

void AGun::SetMaxAmmo(int value)
{
	MaxAmmo = FMath::Max(value, 0);
}

int AGun::GetMaxAmmo()
{
	return FMath::Max(MaxAmmo, 0);
}

void AGun::SetMinDamage(int value)
{
	MinDamage = FMath::Max(MinDamage, 0);
}

int AGun::GetMinDamage()
{
	return FMath::Max(MinDamage, 0);
}

void AGun::SetMaxDamage(int value)
{
	MaxDamage = FMath::Max(value, 0);
}

int AGun::GetMaxDamage()
{
	return FMath::Max(MaxDamage, 0);
}

void AGun::SetCritChance(float value)
{
	CritChance = FMath::Clamp(value, 0.0f, 100.0f);
}

float AGun::GetCritChance()
{
	return FMath::Clamp(CritChance, 0.0f, 100.0f);
}

void AGun::SetCritMultiplier(float value)
{
	CritMultiplier = FMath::Max(0.0f, value);
}

float AGun::GetCritMultiplier()
{
	return FMath::Max(0.0f, CritMultiplier);
}



