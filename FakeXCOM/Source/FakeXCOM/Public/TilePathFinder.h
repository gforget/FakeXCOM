// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericStack.h"
#include "UObject/Object.h"
#include "TilePathFinder.generated.h"

class UNodePath;
class ASoldier;
class UTileMovementComponent;
/**
 * 
 */
UCLASS()
class FAKEXCOM_API UTilePathFinder : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bCanMoveUnit = true;

	UFUNCTION()
	void OnUnitStartMovingEvent();

	UFUNCTION()
	void OnUnitStopMovingEvent();
	
	void SubscribeOnUnitStartMovingEvent(UTileMovementComponent* UnitMovementComponent);
	
	GenericStack<UNodePath*> GetPathToDestination(UNodePath* InitialNode, UNodePath* DestinationNode);
	
	void AddNodeToCameFrom(TMap<int, UNodePath*>& came_from, int IdNode, UNodePath* ValueNode);
	UNodePath* GetNodeFromCameFrom(TMap<int, UNodePath*>& came_from, int IdNode);
	
	void AddCostToCostSoFar(TMap<int, float>& cost_so_far,int IdNode, float Cost);
	float GetCostFromCostSoFar(TMap<int, float>& cost_so_far, int IdNode);

	void MoveUnit(const ASoldier* Soldier, UNodePath* ChosenNode);
};
