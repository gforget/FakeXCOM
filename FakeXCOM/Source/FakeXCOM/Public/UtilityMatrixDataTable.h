// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UtilityMatrixDataTable.generated.h"

class UConsideration;
class UAIAbility;
/**
 * 
 */

UENUM(BlueprintType)
enum EAIAbilityTargetType
{
	None,
	Actor,
	NodePath
};

USTRUCT(BlueprintType)
struct FUtilityMatrixDT : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FUtilityMatrixDT(){}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	bool IsEnabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TSubclassOf<UAIAbility> ActionAbility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TArray<UConsideration*> ActionConsiderations;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TEnumAsByte<EAIAbilityTargetType> TargetType = None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (EditCondition = "TargetType != EAIAbilityTargetType::None"))
	TArray<UConsideration*> TargetConsiderations;
	
};


UCLASS()
class FAKEXCOM_API UUtilityMatrixDataTable : public UDataTable
{
	GENERATED_BODY()
};
