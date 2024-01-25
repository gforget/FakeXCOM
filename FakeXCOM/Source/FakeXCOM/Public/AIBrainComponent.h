﻿// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UtilityMatrixDataTable.h"
#include "AIBrainComponent.generated.h"

class ATBTacticalGameMode;
class AUnit;
class UNodePath;
class UConsideration;
class UAIAbility;
class UUtilityMatrixDataTable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UAIBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIBrainComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	
	UPROPERTY(BlueprintReadOnly)
	UNodePath* TargetNodePath;

	UPROPERTY(BlueprintReadOnly)
	int TargetActorIndex;
	
	TArray<UNodePath*> AllBaseDistanceNode;
	TArray<UNodePath*> AllLongDistanceNode;
	
	UPROPERTY(EditDefaultsOnly)
	float DelayBetweenDecision = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility Matrix", meta = (RowType = "FUtilityMatrixDT"))
	UDataTable* UtilityMatrix;

	UPROPERTY()
	FTimerHandle DecideActionTimerHandle;
	void DecideActionTimerFunction();
	
	TSubclassOf<UAIAbility> DecideBestAction();
	
	float ScoreAction(TArray<UConsideration*> Considerations);
	
	UNodePath* PickNodePath(FUtilityMatrixDT* UMRow);
	float ScoreNodePath(TArray<UConsideration*> Considerations, UNodePath* Node);

	int PickTargetActor(FUtilityMatrixDT* UMRow);
	
	float ScoreTargetActor(TArray<UConsideration*> Considerations, AActor* Actor);

	UPROPERTY(EditDefaultsOnly, Category = "Debug Properties")
	bool bLogActionScore = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Properties")
	bool bShowNodeScore = false;

	UPROPERTY(EditDefaultsOnly, Category = "Debug Properties")
	bool bShowTargetActorScore = false;
	
private:

	UFUNCTION()
	void OnUnitSelected(AUnit* Unit);
	
	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;

	UPROPERTY()
	AUnit* OwningUnit;
};
