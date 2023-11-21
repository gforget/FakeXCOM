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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitOrderedToMoveEvent, AUnit*, Unit);

UCLASS()
class FAKEXCOM_API UUnitManager : public UObject
{
	GENERATED_BODY()
public :

	UPROPERTY(BlueprintAssignable)
	FUnitSelectedEvent OnUnitSelectedEvent;

	UPROPERTY(BlueprintAssignable)
	FUnitOrderedToMoveEvent OnUnitOrderedToMoveEvent;
	
	UPROPERTY()
	int SelectedUnitId = -1;

	void AddUnitToManager(int IdUnit, AUnit* Unit);
	
	UPROPERTY(BlueprintReadOnly)
	TMap<int, AUnit*> AllUnitReference;

	void SelectNextUnit();
	void SelectPreviousUnit();

	AUnit* SelectUnit(int UnitId, bool bGoToUnit = true);
	AUnit* GetCurrentlySelectedUnit();

	ATBTacticalGameMode* TBTacticalGameMode;
	void Initialize(ATBTacticalGameMode* TBTacticalGameModePtr);

private:

	UFUNCTION()
	void OnRightClickSelectActor(AActor* Actor, FVector HitLocation);
	
	UFUNCTION()
	void OnLeftClickSelectActor(AActor* Actor, FVector HitLocation);
	
	UFUNCTION()
	void OnUnitRanOutOfActions(AUnit* Unit);
};
