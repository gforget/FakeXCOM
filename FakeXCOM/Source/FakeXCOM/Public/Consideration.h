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

	UFUNCTION(BlueprintNativeEvent, Category = "Main Functions")
	float ScoreConsiderationNode(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode, UNodePath* TargetNode);

	UFUNCTION(BlueprintNativeEvent, Category = "Main Functions")
	float ScoreConsiderationActor(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode, AActor* TargetActor);

	//Node consideration function
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	float GetLowestPotentialDefence(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode, UNodePath* TargetNode);

	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	int GetNumberOfEnemyInSight(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode, UNodePath* TargetNode);

private:
	TArray<AUnit*> GetAllEnemyUnits(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode);
};
