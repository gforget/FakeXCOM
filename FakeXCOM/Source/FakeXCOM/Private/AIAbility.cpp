// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAbility.h"
#include "TBTacticalGameMode.h"

ATBTacticalGameMode* UAIAbility::GetTBTacticalGameMode()
{
	return GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
}
