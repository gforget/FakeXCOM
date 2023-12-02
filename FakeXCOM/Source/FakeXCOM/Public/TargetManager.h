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
UENUM(BlueprintType)

enum ETargetType
{
	Self,
	SingleTarget,
	SelfAOE,
	SingleTargetAOE, 
	GroundAOE,
	SingleGroundPosition
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
	int SelectedUnitIndex = -1;
	
	UPROPERTY(BlueprintReadWrite, category = "Data")
	TArray<AActor*> AllAvailableTarget;

	void Initialize(ATBTacticalGameMode* TBTacticalGameModeRef);

	void SelectNextTarget();
	void SelectPreviousTarget();
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void SelectTarget(int TargetIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	AActor* GetTargetFromIndex(int TargetIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void UpdateTargetsUsingLineOfSight(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFaction>> ValidFactions,
		float LineOfSightRange
		);
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void UpdateTargetsUsingMeleeRange(
		AUnit* SeekingUnit,
		TArray<TEnumAsByte<EFaction>> ValidFactions
	); 	
};

