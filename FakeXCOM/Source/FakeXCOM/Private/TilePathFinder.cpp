﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "TilePathFinder.h"
#include "DebugHeader.h"
#include "GenericStack.h"
#include "GnericPriorityQueue.h"
#include "NodePath.h"

GenericStack<UNodePath*> UTilePathFinder::GetPathToDestination(UNodePath* DestinationNode)
{
	UNodePath* initialNode = StartingNode;

	GenericPriorityQueue<UNodePath*, float> frontier;
	frontier.Enqueue(initialNode, 0.0f);

	TMap<int, UNodePath*> came_from;
	AddNodeToCameFrom(came_from, initialNode->IdNode, nullptr);
	
	TMap<int, float> cost_so_far;
	AddCostToCostSoFar(cost_so_far, initialNode->IdNode, 0.0f);

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
			
			if (!cost_so_far.Contains(next->IdNode) || new_cost < GetCostFromCostSoFar(cost_so_far, next->IdNode))
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
	
	DebugScreen(FString::FromInt(came_from.Num()), FColor::Red);

	GenericStack<UNodePath*> PathStack;
	if (bPathFound)
	{
		UNodePath* current = DestinationNode;
		while(current->IdNode != initialNode->IdNode)
		{
			PathStack.Push(current);
			current = GetNodeFromCameFrom(came_from, current->IdNode);
		}
	}
	
	return PathStack;
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
