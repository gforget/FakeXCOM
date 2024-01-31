// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

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
	float ScoreConsideration(AUnit* OwningUnit, int ActionIndex, ATBTacticalGameMode* TBTacticalGameMode);

	UFUNCTION(BlueprintNativeEvent, Category = "Main Functions")
	float ScoreConsiderationNode(AUnit* OwningUnit, int ActionIndex, ATBTacticalGameMode* TBTacticalGameMode, UNodePath* TargetNode);

	UFUNCTION(BlueprintNativeEvent, Category = "Main Functions")
	float ScoreConsiderationActor(AUnit* OwningUnit, int ActionIndex, ATBTacticalGameMode* TBTacticalGameMode, AActor* TargetActor);

	//Node consideration function
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	float GetLowestPotentialDefence(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode, UNodePath* TargetNode);

	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	TArray<AUnit*> GetAllEnemyUnitInSight(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode, UNodePath* TargetNode);

private:
	TArray<AUnit*> GetAllEnemyUnits(AUnit* OwningUnit, ATBTacticalGameMode* TBTacticalGameMode);
};
