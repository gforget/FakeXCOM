// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "GameplayAbilities//AIAbility.h"

#include "AbilitySystemComponent.h"
#include "TBTacticalGameMode.h"
#include "GameplayAbilities/UnitAbility.h"

ATBTacticalGameMode* UAIAbility::GetTBTacticalGameMode()
{
	return GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
}
