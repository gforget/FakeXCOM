// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UnitManager.generated.h"

class ATBTacticalGameMode;
class UNodePath;
/**
 * 
 */

UENUM(BlueprintType)
enum EFaction
{
	Player,
	Ally,
	Enemy,
	Neutral
};

USTRUCT(BlueprintType)
struct NO_API FUnitFactionStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int> UnitInFaction;
	
	FUnitFactionStruct()
	{
		// Initialize the struct members with default values
		UnitInFaction = TArray<int>();
	}
	
	FUnitFactionStruct(TArray<int> _UnitFactionMap)
	{
		UnitInFaction = _UnitFactionMap;
	}
};

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
	int SelectedUnitIndex = -1;

	UPROPERTY()
	TEnumAsByte<EFaction> SelectedFaction = EFaction::Player;
	
	void AddUnitToManager(int IdUnit, AUnit* Unit);

	TMap<int, AUnit*> AllUnitReference;
	
	void SelectNextUnit();
	void SelectPreviousUnit();

	AUnit* SelectUnit(int UnitId, bool bGoToUnit = true);
	AUnit* GetCurrentlySelectedUnit();

	ATBTacticalGameMode* TBTacticalGameMode;
	
	void Initialize(ATBTacticalGameMode* TBTacticalGameModePtr);

	UFUNCTION(BlueprintCallable)
	void EndOfAbility();
	
private:

	UPROPERTY()
	TMap<TEnumAsByte<EFaction>, FUnitFactionStruct> AllUnitFactionReferenceMap;
	
	UPROPERTY()
	bool bAllUnitOutOfAction = false;
	
	UFUNCTION()
	void OnRightClickSelectActor(AActor* Actor, FVector HitLocation);
	
	UFUNCTION()
	void OnLeftClickSelectActor(AActor* Actor, FVector HitLocation);
	
	UFUNCTION()
	void OnUnitRanOutOfActions(AUnit* Unit);

	void MovementActionCost(const UNodePath* Destination);
};
