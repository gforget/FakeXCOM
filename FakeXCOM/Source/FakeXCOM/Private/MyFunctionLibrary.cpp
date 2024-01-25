// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "MyFunctionLibrary.h"

float UMyFunctionLibrary::GetRuntimeFloatCurveValue(const FRuntimeFloatCurve& InCurve, float InTime)
{
	// Evaluate the curve
	return InCurve.GetRichCurveConst()->Eval(InTime);
}
