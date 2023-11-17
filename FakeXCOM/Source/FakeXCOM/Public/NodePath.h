﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "LevelBlock.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NodePath.generated.h"

USTRUCT(BlueprintType)
struct NO_API FCoverInfo
{
	GENERATED_BODY()
	
	// You can define your struct members here
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool FullCover = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector IconPosition = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator IconRotation = FRotator::ZeroRotator;
	
	FCoverInfo()
	{
		// Initialize the struct members with default values
		FullCover = false;
		IconPosition = FVector::ZeroVector;
		IconRotation = FRotator::ZeroRotator;
	}
	
	FCoverInfo(bool _FullCover, const FVector& _IconPosition, const FRotator& _IconRotation)
	{
		FullCover = _FullCover;
		IconPosition = _IconPosition;
		IconRotation = _IconRotation;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UNodePath : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNodePath();
	
	void Initialize();
	
	UPROPERTY()
	TArray<UNodePath*> AllNeighbours;

	UPROPERTY()
	TArray<float> AllNeighboursBaseCost;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NodePath Properties")
	TArray<FCoverInfo> AllCoverInfos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NodePath Properties")
	int IdNode = -1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NodePath Properties")
	int NbNeighbour;

	UPROPERTY()
	bool bIsBlocked = false;
	
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
