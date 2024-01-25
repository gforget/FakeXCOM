// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AIAbility.generated.h"

/**
 * 
 */
class ATBTacticalGameMode;

UCLASS()
class FAKEXCOM_API UAIAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category="Main Functions")
	ATBTacticalGameMode* GetTBTacticalGameMode();
};
