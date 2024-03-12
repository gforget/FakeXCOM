// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Managers/FactionManager.h"
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

UENUM(BlueprintType)
enum EDeadTargetFilter
{
	NoDeadTarget,
	OnlyDeadTarget,
	AllTarget
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTargetSelectedEvent, int, TargetIndex, AUnit*, SelectingUnit);

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
	void UpdateAllCurrentAvailableTargetWithAbilityTargets(AUnit* OwningUnit, const FString& UnitAbilityId);
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void SelectTarget(int TargetIndex, bool bMoveToActor = true);

	UFUNCTION(BlueprintPure, Category = "Main Functions")
	AActor* GetCurrentlySelectedTarget();
	
	UFUNCTION(BlueprintPure, Category = "Main Functions")
	AActor* GetTargetFromIndex(int TargetIndex);

	UFUNCTION(BlueprintPure, Category = "Main Functions")
	TArray<int> GetAllValidUnitIdFromFactionRelation(AUnit* SeekingUnit, TArray<TEnumAsByte<EFactionRelation>> ValidRelations);
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	bool ValidateTargetDeathFilter(AUnit* PotentialTarget, TEnumAsByte<EDeadTargetFilter> DeadTargetFilter);
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void GetAllAvailableTargetsBaseOnAbilityProperties(UUnitAbility* UnitAbility, TArray<AUnit*>& AllAvailableUnitTargets);

	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	bool ConfirmLineOfSightOnUnit(
		AUnit* SeekingUnit,
		AUnit* TargetUnit,
		FVector SeekingUnitPosition,
		FVector TargetUnitPosition,
		float LineOfSightRange);
	
private:
	void GetTargetsFromAbiiltyRange(UUnitAbility* UnitAbility, TArray<AUnit*>& AllAvailableUnitTargets);
	
	void GetTargetsInRange(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
		float Range,
		TEnumAsByte<EDeadTargetFilter> DeadTargetFilter,
		TArray<AUnit*>& AllAvailableUnitTargets
		);
	
	void GetTargetsInRangeUsingLineOfSight(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
		float LineOfSightRange,
		TEnumAsByte<EDeadTargetFilter> DeadTargetFilter,
		TArray<AUnit*>& AllAvailableUnitTargets
		);
	
	void  GetTargetsUsingMeleeRange(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFactionRelation>> ValidFactionsRelation,
		TEnumAsByte<EDeadTargetFilter> DeadTargetFilter,
		TArray<AUnit*>& AllAvailableUnitTargets
	);
	
};

