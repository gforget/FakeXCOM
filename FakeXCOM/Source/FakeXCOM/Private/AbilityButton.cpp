// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityButton.h"

#include "AbilitySystemComponent.h"
#include "Unit.h"

UAbilityButton::UAbilityButton()
{
	OnClicked.AddDynamic(this, &UAbilityButton::OnClick);
	CallAbility.AddDynamic(this,&UAbilityButton::OnCallAbility);
}

UAbilityButton::~UAbilityButton()
{
	UnbindEvents();
}

void UAbilityButton::OnClick()
{
	CallAbility.Broadcast();
}

void UAbilityButton::OnCallAbility()
{
	if (UnitRef)
	{
		UnitRef->AbilitySystemComponent->TryActivateAbility(AbilityHandle);
	}
}

void UAbilityButton::UnbindEvents()
{
	OnClicked.RemoveDynamic(this, &UAbilityButton::OnClick);
	CallAbility.RemoveDynamic(this,&UAbilityButton::OnCallAbility);
}
