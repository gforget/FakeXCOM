// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFunctionLibrary.h"

float UMyFunctionLibrary::GetRuntimeFloatCurveValue(const FRuntimeFloatCurve& InCurve, float InTime)
{
	// Evaluate the curve
	return InCurve.GetRichCurveConst()->Eval(InTime);
}
