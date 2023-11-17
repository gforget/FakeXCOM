// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/Button.h"
#include "AbilityButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCallAbilityDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

class AUnit;
UCLASS()
class FAKEXCOM_API UAbilityButton : public UButton
{
	GENERATED_BODY()
	
public :
	UAbilityButton();
	
	~UAbilityButton();
	
	UPROPERTY(BlueprintReadWrite)
	AUnit* UnitRef;
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayAbilitySpecHandle AbilityHandle;
	
	UPROPERTY()
	FCallAbilityDelegate CallAbility;
	
	UPROPERTY()
	FClickDelegate Click;

	UFUNCTION()
	void OnClick();
	
	UFUNCTION()
	void OnCallAbility();

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void UnbindEvents();
};
