// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "AbilitySystemComponent.h"
#include "GunAttributeSet.h"
// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
}

UAbilitySystemComponent* AGun::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		GunAttributeSet = ASC->GetSet<UGunAttributeSet>();
	}
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

