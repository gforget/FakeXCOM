// Fill out your copyright notice in the Description page of Project Settings.


#include "TilePathFinder.h"
#include "GenericStack.h"
#include "..\Public\GenericPriorityQueue.h"
#include "NodePath.h"
#include "TileMovementComponent.h"
#include "UnitAttributeSet.h"

void UTilePathFinder::SubscribeOnUnitMovingEvents(UTileMovementComponent* UnitMovementComponent)
{
	UnitMovementComponent->OnUnitStartMovingEvent.AddDynamic(this, &UTilePathFinder::OnUnitStartMovingEvent);
	UnitMovementComponent->OnUnitStopMovingEvent.AddDynamic(this, &UTilePathFinder::OnUnitStopMovingEvent);
}

void UTilePathFinder::OnUnitStartMovingEvent(AActor* MovingActor)
{
	bCanMoveUnit = false;
}

void UTilePathFinder::OnUnitStopMovingEvent(AActor* MovingActor)
{
	bCanMoveUnit = true;
}

GenericStack<UNodePath*> UTilePathFinder::GetPathToDestination(UNodePath* InitialNode, UNodePath* DestinationNode)
{
	
	GenericPriorityQueue<UNodePath*, float> frontier;
	frontier.Enqueue(InitialNode, 0.0f);

	TMap<int, UNodePath*> came_from;
	AddNodeToCameFrom(came_from, InitialNode->IdNode, nullptr);
	
	TMap<int, float> cost_so_far;
	AddCostToCostSoFar(cost_so_far, InitialNode->IdNode, 0.0f);

	bool bPathFound = false;
	int iteration = 0;
	while (!frontier.IsEmpty())
	{
		UNodePath* current = frontier.Dequeue();
		for (int i = 0; i<current->AllNeighbours.Num(); i++)
		{
			iteration++;
			UNodePath* next = current->AllNeighbours[i];
			const float new_cost = GetCostFromCostSoFar(cost_so_far, current->IdNode) + current->AllNeighboursBaseCost[i]*next->WeightCost;
			
			if ((!cost_so_far.Contains(next->IdNode) || new_cost < GetCostFromCostSoFar(cost_so_far, next->IdNode))
				&& !next->bIsBlocked )
			{
				AddCostToCostSoFar(cost_so_far, next->IdNode, new_cost);
				AddNodeToCameFrom(came_from, next->IdNode, current);
				
				frontier.Enqueue(next, new_cost);
				if (next->IdNode == DestinationNode->IdNode)
				{
					bPathFound = true;
					break;
				}
			}
		}

		if (bPathFound)
		{
			break;
		}
	}
	
	//Generate the path
	GenericStack<UNodePath*> PathStack;
	if (bPathFound)
	{
		UNodePath* current = DestinationNode;
		while(current->IdNode != InitialNode->IdNode)
		{
			PathStack.Push(current);
			current = GetNodeFromCameFrom(came_from, current->IdNode);
		}
	}
	
	// clear memory to avoid memory leak
	frontier.Clear(); 
	came_from.Empty();
	cost_so_far.Empty();
	
	return PathStack;
}
void UTilePathFinder::GetNodeDistanceLimitForUnit(AUnit* Unit, TArray<UNodePath*>& AllBaseDistanceNode,
	TArray<UNodePath*>& AllLongDistanceNode)
{
	const int BaseDistance = Unit->UnitAttributeSet->GetMaxMoveDistancePerAction();
	const int LongDistance = BaseDistance*2;
	
	UNodePath* InitialNode = Unit->TileMovementComponent->LocatedNodePath;
	
	GenericPriorityQueue<UNodePath*, float> frontier;
	frontier.Enqueue(InitialNode, 0.0f);

	TMap<int, UNodePath*> came_from;
	AddNodeToCameFrom(came_from, InitialNode->IdNode, nullptr);
	
	TMap<int, float> cost_so_far;
	AddCostToCostSoFar(cost_so_far, InitialNode->IdNode, 0.0f);
	
	int iteration = 0;
	while (!frontier.IsEmpty())
	{
		iteration++;
		UNodePath* current = frontier.Dequeue();
		for (int i = 0; i<current->AllNeighbours.Num(); i++)
		{
			UNodePath* next = current->AllNeighbours[i];
			const float new_cost = GetCostFromCostSoFar(cost_so_far, current->IdNode) + current->AllNeighboursBaseCost[i]*next->WeightCost;
			
			if ((!cost_so_far.Contains(next->IdNode) || new_cost < GetCostFromCostSoFar(cost_so_far, next->IdNode))
				&& !next->bIsBlocked )
			{
				AddCostToCostSoFar(cost_so_far, next->IdNode, new_cost);
				AddNodeToCameFrom(came_from, next->IdNode, current);
				frontier.Enqueue(next, new_cost);
				
				if (iteration == BaseDistance)
				{
					AllBaseDistanceNode.Add(next);
				}
				else if (iteration == LongDistance)
				{
					AllLongDistanceNode.Add(next);
				}
			}
		}

		if (iteration == LongDistance)
		{
			break;
		}
	}
	
	// clear memory to avoid memory leak
	frontier.Clear(); 
	came_from.Empty();
	cost_so_far.Empty();
}

void UTilePathFinder::AddNodeToCameFrom(TMap<int, UNodePath*>& came_from, int IdNode, UNodePath* ValueNode)
{
	if (!came_from.Contains(IdNode))
	{
		came_from.Add(IdNode, ValueNode);
	}
	else
	{
		came_from[IdNode] = ValueNode;
	}
}

UNodePath* UTilePathFinder::GetNodeFromCameFrom(TMap<int, UNodePath*>& came_from, int IdNode)
{
	if (came_from.Contains(IdNode))
	{
		return came_from[IdNode];
	}

	return nullptr;
}

void UTilePathFinder::AddCostToCostSoFar(TMap<int, float>& cost_so_far, int IdNode, float Cost)
{
	if (!cost_so_far.Contains(IdNode))
	{
		cost_so_far.Add(IdNode, Cost);
	}
	else
	{
		cost_so_far[IdNode] = Cost;
	}
}

float UTilePathFinder::GetCostFromCostSoFar(TMap<int, float>& cost_so_far, int IdNode)
{
	if (cost_so_far.Contains(IdNode))
	{
		return cost_so_far[IdNode];
	}

	return 0.0f;
}

void UTilePathFinder::MoveUnit(const AUnit* Unit, UNodePath* ChosenNode)
{
	if (bCanMoveUnit)
	{
		Unit->TileMovementComponent->FollowPath(GetPathToDestination(Unit->TileMovementComponent->LocatedNodePath, ChosenNode));
	}
}


