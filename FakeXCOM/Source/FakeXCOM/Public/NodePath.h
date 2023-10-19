// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "LevelBlock.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NodePath.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UNodePath : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNodePath();
	
	void Initialize();

	// reference to all neighbour
	UPROPERTY()
	TArray<UNodePath*> AllNeighbours;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NodePath Properties")
	int nbNeighbour;
	
	// weight cost
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NodePath Properties")
	float WeightCost = 1.0f;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool TryConnectNeighbour(int IndexDirection, TArray<FVector>& DirectionVectors, TArray<bool>& foundBlock, TArray<bool>& foundVoid, UNodePath* ChosenNodePath, ALevelBlock* LevelBlockPtr, ALevelBlock* NeighbourLevelBlock);
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
