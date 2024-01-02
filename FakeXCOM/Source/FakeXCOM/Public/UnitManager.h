// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionManager.h"
#include "UObject/Object.h"
#include "UnitManager.generated.h"

class ATBTacticalGameMode;
class UNodePath;
/**
 * 
 */

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
	
	FUnitFactionStruct(const TArray<int>& _UnitFactionArray)
	{
		UnitInFaction = _UnitFactionArray;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitSpawnedEvent, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitSelectedEvent, AUnit*, Unit);

UCLASS()
class FAKEXCOM_API UUnitManager : public UObject
{
	
	GENERATED_BODY()
	
public :
	
	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY(BlueprintAssignable)
	FUnitSpawnedEvent OnUnitSpawnedEvent;
	
	UPROPERTY(BlueprintAssignable)
	FUnitSelectedEvent OnUnitSelectedEvent;

	UPROPERTY()
	TMap<int, AUnit*> AllUnitReference;
	
	UPROPERTY()
	int SelectedUnitIndex = -1;

	TArray<int> GetAllUnitsIdFromFactions(EFaction Faction);
	
	void AddUnitToManager(int IdUnit, AUnit* Unit);
	
	void SelectNextUnit();
	void SelectPreviousUnit();
	
	AUnit* SelectUnit(int UnitId, bool bGoToUnit = true);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	AUnit* GetUnitFromId(int UnitId);
	
	AUnit* GetCurrentlySelectedUnit();
	
	void Initialize(ATBTacticalGameMode* TBTacticalGameModePtr);

	UFUNCTION(BlueprintCallable)
	void EndOfAbility();
	
	void SelectFirstUnitOfFaction(EFaction Faction);
	void ResetAllActionsOfFaction(EFaction Faction);
	
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
};
