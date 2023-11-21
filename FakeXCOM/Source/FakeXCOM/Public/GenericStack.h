#pragma once

#include "CoreMinimal.h"
#include "DebugHeader.h"

/**
 * A generic stack class that can store values of any data type.
 */
template <typename T>
class GenericStack
{
public:
	// Constructor
	GenericStack() {}

	// Push an element onto the stack
	void Push(T Element)
	{
		Stack.Add(Element);
	}

	// Pop an element from the stack
	T Pop()
	{
		if (Stack.Num() > 0)
		{
			T Element = Stack.Last();
			Stack.RemoveAt(Stack.Num() - 1);
			return Element;
		}
		// Handle stack underflow (stack is empty)
		else
		{
			// You can throw an exception, print an error message, or return a default value here.
			// In this example, we'll return a default-constructed value.
			DebugScreen(TEXT("Stack is empty"), FColor::Red);
			return T();
		}
	}

	// Check if the stack is empty
	bool IsEmpty()
	{
		return Stack.Num() == 0;
	}

	// Get the size of the stack
	int32 Num()
	{
		return Stack.Num();
	}

	// Clear the stack
	void Clear()
	{
		Stack.Empty();
	}

private:
	TArray<T> Stack;
};

