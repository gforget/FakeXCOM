// Fill out your copyright notice in the Description page of Project Settings.


#include "GunAttributeSet.h"

#include "Gun.h"

float UGunAttributeSet::GetAmmo() const
{
	return FMath::Max(Ammo.GetCurrentValue(), 0.0f);
}

void UGunAttributeSet::SetAmmo(float NewVal) const
{
	NewVal = FMath::Clamp(NewVal, 0.0f, MaxAmmo.GetCurrentValue());
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetAmmoAttribute(), NewVal);
		Cast<AGun>(GetOwningActor())->CallAmmoChanged();
	}
}

float UGunAttributeSet::GetMaxAmmo() const
{
	return FMath::Max(MaxAmmo.GetCurrentValue(), 0.0f);
}

void UGunAttributeSet::SetMaxAmmo(float NewVal) const
{
	NewVal = FMath::Max(NewVal, 0.0f);
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetMaxAmmoAttribute(), NewVal);
	}
}

void UGunAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	//TODO: maybe this can be useful when using gameplay effect, have to see.
	//TODO: getter and setter are already clamping the value.
}
