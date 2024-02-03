// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/Button.h"
#include "AbilityButton.generated.h"

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
	FString AbilityID;

	UFUNCTION()
	void OnClick();

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void UnbindEvents();
};
