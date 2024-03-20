// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"

#define CHECK_NULL_POINTER(Pointer) CheckNullPointer(Pointer, #Pointer, __FUNCTION__)

inline void DebugScreen(const FString& Message, const FColor& Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, Color, Message);
		if (Color == FColor::Red)
		{
			UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}
}

template<typename T>
bool CheckNullPointer(const T* Pointer,  const char* PointerName, const char* FunctionName)
{
	if (!Pointer)
	{
		DebugScreen(FString(UTF8_TO_TCHAR(PointerName)) + " is null in "+ FunctionName, FColor::Red);
		return true;
	}
	
	return false;
}

