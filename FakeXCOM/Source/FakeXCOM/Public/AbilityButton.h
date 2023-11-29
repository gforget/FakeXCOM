// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/Button.h"
#include "AbilityButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCallAbilityDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

class AUnit;
class ULevelUI;

UCLASS()
class FAKEXCOM_API UAbilityButton : public UButton
{
	GENERATED_BODY()
	
public :
	UAbilityButton();

	virtual ~UAbilityButton() override;
	
	UPROPERTY(BlueprintReadWrite)
	AUnit* UnitRef;

	UPROPERTY(BlueprintReadWrite)
	ULevelUI* LevelUIRef;
	
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
