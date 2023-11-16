// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"

float UUnitAttributeSet::GetHealth() const
{
	return FMath::Max(Health.GetCurrentValue(), 0.0f);
}

void UUnitAttributeSet::SetHealth(float NewVal) const
{
	NewVal = FMath::Clamp(NewVal, 0.0f, MaxHealth.GetCurrentValue());
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetHealthAttribute(), NewVal);
	}
}

float UUnitAttributeSet::GetMaxHealth() const
{
	return FMath::Max(MaxHealth.GetCurrentValue(), 0.0f);
}

void UUnitAttributeSet::SetMaxHealth(float NewVal) const
{
	NewVal = FMath::Max(NewVal, 0.0f);
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetMaxHealthAttribute(), NewVal);
	}
}

float UUnitAttributeSet::GetActions() const
{
	return FMath::Max(Actions.GetCurrentValue(), 0.0f);
}

void UUnitAttributeSet::SetActions(float NewVal) const
{
	NewVal = FMath::Clamp(NewVal, 0.0f, MaxActions.GetCurrentValue());
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetActionsAttribute(), NewVal);
	}
}

float UUnitAttributeSet::GetMaxActions() const
{
	return FMath::Max(MaxActions.GetCurrentValue(), 0.0f);
}

void UUnitAttributeSet::SetMaxActions(float NewVal) const
{
	NewVal = FMath::Max(NewVal, 0.0f);
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		ASC->SetNumericAttributeBase(GetMaxActionsAttribute(), NewVal);
	}
}

void UUnitAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	//TODO: maybe this can be useful when using gameplay effect, have to see.
	//TODO: getter and setter are already clamping the value.
	
	// // Check to see if this call affects our Health by using the Property Getter.
	// if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	// {
	// 	// This Gameplay Effect is changing Health. Apply it, but restrict the value first.
	// 	// In this case, Health's base value must be non-negative.
	// 	SetHealth(FMath::Max(GetHealth(), 0.0f));
	// }
}
