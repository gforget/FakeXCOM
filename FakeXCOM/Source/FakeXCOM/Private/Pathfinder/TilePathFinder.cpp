// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "Pathfinder/TilePathFinder.h"

#include "TBTacticalGameMode.h"

#include "AttributeSets/UnitAttributeSet.h"
#include "AI/AIBrainComponent.h"
#include "Managers/UnitAbilityManager.h"
#include "Pathfinder/NodePath.h"
#include "Pathfinder/TileMovementComponent.h"
#include "Utility/GenericPriorityQueue.h"
#include "Utility/GenericStack.h"

void UTilePathFinder::SubscribeOnUnitMovingEvents(UTileMovementComponent* UnitMovementComponent)
{
	if (CHECK_NULL_POINTER(UnitMovementComponent)) return;
	UnitMovementComponent->OnUnitStartMovingEvent.AddDynamic(this, &UTilePathFinder::OnUnitStartMovingEvent);
	UnitMovementComponent->OnUnitStopMovingEvent.AddDynamic(this, &UTilePathFinder::OnUnitStopMovingEvent);
}

void UTilePathFinder::SubscribeGameModeEvent(ATBTacticalGameMode* TBTacticalGameMode)
{
	if (CHECK_NULL_POINTER(TBTacticalGameMode)) return;
	TBTacticalGameMode->UnitAbilityManager->OnAbilitySelectionModeChangeEvent.AddDynamic(this, &UTilePathFinder::OnAbilitySelectionModeChangeEvent);
}

void UTilePathFinder::OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue)
{
	if (AbilitySelectionModeValue == true)
	{
		bCanMoveUnit = false;	
	}
	else
	{
		bCanMoveUnit = true;
	}
}

void UTilePathFinder::OnUnitStartMovingEvent(AActor* MovingActor)
{
	if (CHECK_NULL_POINTER(MovingActor)) return;
	bCanMoveUnit = false;
}

void UTilePathFinder::OnUnitStopMovingEvent(AActor* MovingActor)
{
	if (CHECK_NULL_POINTER(MovingActor)) return;
	bCanMoveUnit = true;
}

GenericStack<UNodePath*> UTilePathFinder::GetPathToDestination(UNodePath* InitialNode, UNodePath* DestinationNode)
{
	if (CHECK_NULL_POINTER(InitialNode)) return GenericStack<UNodePath*>();
	if (CHECK_NULL_POINTER(DestinationNode)) return GenericStack<UNodePath*>();
	
	GenericStack<UNodePath*> PathStack;
	if (!InitialNode || !DestinationNode)
	{
		DebugScreen("Node parameters send to pathfinding are empty", FColor::Red);
		return PathStack;
	}
	
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
		for (int i = 0; i<current->AllConnectedNeighbours.Num(); i++)
		{
			iteration++;
			UNodePath* next = current->AllConnectedNeighbours[i];
			if (!next)
			{
				DebugScreen("Current node"+ current->GetName() +" neighbour " + FString::FromInt(i) + "is Null, please regenerate the pathfinding graph", FColor::Red);
				continue;
			}
			
			const float new_cost = GetCostFromCostSoFar(cost_so_far, current->IdNode) + current->AllConnectedNeighboursBaseCost[i]*next->WeightCost;
			
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

void UTilePathFinder::GetNodeDistanceLimitForUnit(AUnit* Unit,
	TArray<UNodePath*>& AllBaseDistanceNode,
	TArray<UNodePath*>& AllLongDistanceNode,
	int& BaseDistance,
	int& LongDistance)
{
	//Use a breadth first approach instead of Dijkstra, because it allow to guarantee the number of
	//steps is in order while we explore the grid. While the path found are not the same, the number
	//of steps is, since diagonal and vertical movement cost the same amount of step in this game.
	
	if (CHECK_NULL_POINTER(Unit)) return;
			
	//Reset NbStep
	for (int i=0;i<AllNodePaths.Num();i++)
	{
		AllNodePaths[i]->NbSteps = -1;
	}
	
	BaseDistance = Unit->UnitAttributeSet->GetMaxMoveDistancePerAction();
	LongDistance = BaseDistance*2;
	
	UNodePath* InitialNode = Unit->TileMovementComponent->LocatedNodePath;
	
	TQueue<UNodePath*> frontier;
	frontier.Enqueue(InitialNode);

	TMap<int, UNodePath*> came_from;
	AddNodeToCameFrom(came_from, InitialNode->IdNode, nullptr);

	TMap<int, int> step_so_far;
	AddStepToStepSoFar(step_so_far, InitialNode->IdNode, 0);
	
	while (!frontier.IsEmpty())
	{
		UNodePath* current;
		frontier.Dequeue(current);
		
		if (current->NbSteps > LongDistance) // now guarantied to be in order, if the current step is higher than long distance, stop checking
		{
			break;
		}
		
		for (int i = 0; i<8; i++)
		{
			UNodePath* next = current->AllNeighboursConnectionInfo[i];
			if (!next)
			{
				continue;
			}
			
			const int new_step = GetStepFromStepSoFar(step_so_far, current->IdNode) + 1;
			if (!came_from.Contains(next->IdNode) && !next->bIsBlocked)
			{
				next->NbSteps = new_step;
				
				AddStepToStepSoFar(step_so_far, next->IdNode, new_step);
				AddNodeToCameFrom(came_from, next->IdNode, current);
				frontier.Enqueue(next);
				
				if (new_step <= BaseDistance)
				{
					AllBaseDistanceNode.Add(next);
				}
				else if (new_step <= LongDistance)
				{
					AllLongDistanceNode.Add(next);
				}
			}
		}
	}
	
	//Copy nodepath reference for AI
	Unit->AIBrainComponent->AllBaseDistanceNode = AllBaseDistanceNode;
	Unit->AIBrainComponent->AllLongDistanceNode = AllLongDistanceNode;
	
	// clear memory to avoid memory leak
	frontier.Empty();
	came_from.Empty();
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

void UTilePathFinder::AddStepToStepSoFar(TMap<int, int>& step_so_far, int IdNode, int Step)
{
	if (!step_so_far.Contains(IdNode))
	{
		step_so_far.Add(IdNode, Step);
	}
	else
	{
		step_so_far[IdNode] = Step;
	}
}

int UTilePathFinder::GetStepFromStepSoFar(TMap<int, int>& step_so_far, int IdNode)
{
	if (step_so_far.Contains(IdNode))
	{
		return step_so_far[IdNode];
	}

	return 0;
}

void UTilePathFinder::MoveUnit(const AUnit* Unit, GenericStack<UNodePath*> Path, bool CallEndOfAbility)
{
	if (CHECK_NULL_POINTER(Unit)) return;
	if (bCanMoveUnit)
	{
		Unit->TileMovementComponent->FollowPath(Path, CallEndOfAbility);
	}
}


