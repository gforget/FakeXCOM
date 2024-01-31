// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/Button.h"
#include "ActivateAbilityButton.generated.h"

/**
 * 
 */
class AUnit;

UCLASS()
class FAKEXCOM_API UActivateAbilityButton : public UButton
{
	GENERATED_BODY()

public:

	UActivateAbilityButton();
	UPROPERTY(BlueprintReadWrite)
	AUnit* UnitRef;
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayAbilitySpecHandle AbilityHandle;
	
	UFUNCTION()
	void OnClick();
};
