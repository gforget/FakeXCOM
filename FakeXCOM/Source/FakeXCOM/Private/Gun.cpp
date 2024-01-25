// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


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

void AGun::CallAmmoChanged()
{
	OnGunAmmoChangeEvent.Broadcast(this);
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

