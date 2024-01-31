// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utility/DebugHeader.h"

template <typename TElement, typename TKey>
class PriorityElement
{
public:
	TElement Element;
	TKey Priority;

	PriorityElement(TElement InElement, TKey InPriority)
		: Element(InElement), Priority(InPriority)
	{
	}

	bool operator<(const PriorityElement& Other) const
	{
		return Priority < Other.Priority;
	}
};

template <typename TElement, typename TKey>
class GenericPriorityQueue
{
public:
	void Enqueue(TElement Element, TKey Priority)
	{
		PriorityQueue.Add(PriorityElement<TElement, TKey>(Element, Priority));
		PriorityQueue.Sort();
	}

	TElement Dequeue()
	{
		if (IsEmpty())
		{
			DebugScreen(TEXT("Priority Queue is empty"), FColor::Red);
			return TElement(); // Handle empty priority queue
		}

		TElement Element = PriorityQueue[0].Element;
		PriorityQueue.RemoveAt(0);
		return Element;
	}

	bool IsEmpty() const
	{
		return PriorityQueue.Num() == 0;
	}
	
	void Clear()
	{
		PriorityQueue.Empty();
	}
private:
	TArray<PriorityElement<TElement, TKey>> PriorityQueue;
};
