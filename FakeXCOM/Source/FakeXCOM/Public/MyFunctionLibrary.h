// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FAKEXCOM_API UMyFunctionLibrary : public UObject
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintPure, Category = "Math|RuntimeFloatCurve")
	static float GetRuntimeFloatCurveValue(const FRuntimeFloatCurve& InCurve, float InTime);
};
