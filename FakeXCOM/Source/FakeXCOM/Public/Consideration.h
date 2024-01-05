// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Consideration.generated.h"
/**
 * 
 */
class ATBTacticalGameMode;
UCLASS()
class FAKEXCOM_API UConsideration : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Main Functions")
	float ScoreConsideration(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode);
};
