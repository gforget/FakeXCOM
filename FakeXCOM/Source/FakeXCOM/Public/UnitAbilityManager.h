// Fill out your copyright notice in the Description page of Project Settings.

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
	UUnitAbility* CurrentSelectedAbility;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle CurrentSelectedAbilityHandle;

	void ActivateAbilitySelectionMode(UUnitAbility* CurrentSelectedAbilityArg, FGameplayAbilitySpecHandle CurrentSelectedAbilityHandleArg);
	void DeactivateAbilitySelectionMode();
	
	bool GetAbilitySelectionMode();
	
private:
	
	bool bAbilitySelectionMode;
};
