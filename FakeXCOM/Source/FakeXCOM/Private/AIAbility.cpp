// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "AIAbility.h"
#include "TBTacticalGameMode.h"

ATBTacticalGameMode* UAIAbility::GetTBTacticalGameMode()
{
	return GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
}
