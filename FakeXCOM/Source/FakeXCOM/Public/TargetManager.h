// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionManager.h"
#include "UObject/Object.h"
#include "TargetManager.generated.h"

/**
 * 
 */

class ATBTacticalGameMode;
class AUnit;
class UUnitAbility;

UENUM(BlueprintType)

enum ETargetType
{
	Self,
	SingleTarget,
	FreeGroundPosition,
	SpecificGroundPosition
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetSelectedEvent, int, TargetIndex);

UCLASS()
class FAKEXCOM_API UTargetManager : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FTargetSelectedEvent OnTargetSelectedEvent;
	
	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY()
	int SelectedTargetIndex = -1;
	
	UPROPERTY(BlueprintReadWrite, category = "Data")
	TArray<AActor*> AllCurrentAvailableTarget;

	void Initialize(ATBTacticalGameMode* TBTacticalGameModeRef);

	void SelectNextTarget();
	void SelectPreviousTarget();
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void SelectTarget(int TargetIndex);

	UFUNCTION(BlueprintPure, Category = "Main Functions")
	AActor* GetCurrentlySelectedTarget();
	
	UFUNCTION(BlueprintPure, Category = "Main Functions")
	AActor* GetTargetFromIndex(int TargetIndex);

	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	TArray<AActor*> GetAllAvailableTargetsBaseOnAbilityProperties(UUnitAbility* UnitAbility);
	
private:
	TArray<AActor*> GetTargetsFromAbiiltyRange(UUnitAbility* UnitAbility);
	
	TArray<AActor*> GetTargetsInRange(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFaction>> ValidFactions,
		float Range
		);
	
	TArray<AActor*> GetTargetsInRangeUsingLineOfSight(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFaction>> ValidFactions,
		float LineOfSightRange
		);
	
	TArray<AActor*>  GetTargetsUsingMeleeRange(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFaction>> ValidFactions
	); 	
};

