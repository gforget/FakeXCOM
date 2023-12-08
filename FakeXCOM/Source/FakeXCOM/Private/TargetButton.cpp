// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetButton.h"

#include "TargetManager.h"
#include "TBTacticalGameMode.h"

UTargetButton::UTargetButton()
{
	OnClicked.AddDynamic(this, &UTargetButton::OnClick);
}

UTargetButton::~UTargetButton()
{
	UnbindEvents();
}

void UTargetButton::OnClick()
{
	if (const ATBTacticalGameMode* TBTacticalGameModePtr = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>())
	{
		TBTacticalGameModePtr->TargetManager->SelectTarget(TargetActorIndex);
	}
}

void UTargetButton::UnbindEvents()
{
}
