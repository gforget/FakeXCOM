// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "UObject/Object.h"
#include "UnitAbilityManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilitySelectionModeChangeEvent, bool, AbilitySelectionModeValue);
class UUnitAbility;
/**
 * 
 */
UCLASS()
class FAKEXCOM_API UUnitAbilityManager : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FAbilitySelectionModeChangeEvent OnAbilitySelectionModeChangeEvent;
	
	UPROPERTY(BlueprintReadOnly)
	FString CurrentSelectedAbilityID;

	void ActivateAbilitySelectionMode(FString CurrentSelectedAbilityID);
	void DeactivateAbilitySelectionMode();
	
	bool GetAbilitySelectionMode();
	
private:
	
	bool bAbilitySelectionMode;
};
