// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UnitManager.generated.h"

class ATBTacticalGameMode;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitSelectedEvent, AUnit*, Unit);
UCLASS()
class FAKEXCOM_API UUnitManager : public UObject
{
	GENERATED_BODY()
public :
	UPROPERTY(BlueprintAssignable)
	FUnitSelectedEvent OnUnitSelectedEvent;

	UPROPERTY()
	int SelectedUnitId = -1;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<int, AUnit*> AllUnitReference;

	void SelectNextUnit();
	void SelectPreviousUnit();

	AUnit* SelectUnit(int UnitId);
	AUnit* GetCurrentlySelectedUnit();

	ATBTacticalGameMode* TBTacticalGameMode;
	void Initialize(ATBTacticalGameMode* TBTacticalGameModePtr);
};
