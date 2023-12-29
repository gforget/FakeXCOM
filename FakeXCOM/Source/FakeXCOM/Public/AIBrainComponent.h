// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIBrainComponent.generated.h"

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

public :
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetBrainActivation (bool value);
	bool GetBrainActivation();
	
public:
	
	UPROPERTY()
	bool bFinishedDeciding = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility Matrix", meta = (RowType = "FUtilityMatrixDT"))
	UDataTable* UtilityMatrix;

	TSubclassOf<UAIAbility> DecideBestAction();
	float ScoreAction(TArray<UConsideration*> Considerations);

private:
	
	UPROPERTY()
	bool bAIBrainActivated = false;
};
