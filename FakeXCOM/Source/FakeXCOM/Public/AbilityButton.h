// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/Button.h"
#include "AbilityButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

class AUnit;
class ULevelUI;
class UUnitAbility;

UCLASS()
class FAKEXCOM_API UAbilityButton : public UButton
{
	GENERATED_BODY()
	
public :
	UAbilityButton();

	virtual ~UAbilityButton() override;

	UPROPERTY(BlueprintReadWrite)
	UUnitAbility* UnitAbilityRef;
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayAbilitySpecHandle AbilityHandle;
	
	UPROPERTY()
	FClickDelegate Click;

	UFUNCTION()
	void OnClick();

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void UnbindEvents();
};
