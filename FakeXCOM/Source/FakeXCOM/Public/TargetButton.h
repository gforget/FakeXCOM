// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "TargetButton.generated.h"

/**
 * 
 */
UCLASS()
class FAKEXCOM_API UTargetButton : public UButton
{
	GENERATED_BODY()

public:
	UTargetButton();

	virtual ~UTargetButton() override;
	
	UFUNCTION()
	void OnClick();

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void UnbindEvents();
	
	UPROPERTY(BlueprintReadWrite)
	int TargetActorIndex;

	UPROPERTY(BlueprintReadWrite)
	bool SelectDefaultAbility = false;
};
