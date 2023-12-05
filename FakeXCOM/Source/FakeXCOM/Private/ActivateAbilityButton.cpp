// Fill out your copyright notice in the Description page of Project Settings.


#include "ActivateAbilityButton.h"

#include "AbilitySystemComponent.h"
#include "Unit.h"

UActivateAbilityButton::UActivateAbilityButton()
{
	OnClicked.AddDynamic(this, &UActivateAbilityButton::OnClick);
}

void UActivateAbilityButton::OnClick()
{
	if (UnitRef)
	{
		UnitRef->AbilitySystemComponent->TryActivateAbility(AbilityHandle);
	}
}
