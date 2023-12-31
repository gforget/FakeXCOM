﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "GameFramework/Actor.h"
#include "LevelBlock.generated.h"

class UNodePath;
class UArrowComponent;

UCLASS()
class FAKEXCOM_API ALevelBlock : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelBlock();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void Destroyed() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UNodePath* GetClosestNodePathFromLocation (FVector Location);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slope Property")
	bool IsSlope;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slope Property")
	FVector TopSlopePosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Slope Property")
	FVector BottomSlopePosition;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pathfinder")
	TArray<FVector> NodePathPositions;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Spawning Position")
	bool bIsSpawningPosition = false;
	
	UPROPERTY(EditInstanceOnly, meta = (EditCondition = "bIsSpawningPosition"), Category = "Spawning Position")
	TArray<int> SpawningNodePathIndexes;

	UPROPERTY(EditInstanceOnly, meta = (EditCondition = "bIsSpawningPosition"), Category = "Spawning Position")
	TArray<TSubclassOf<AUnit>> UnitClasses;

	UPROPERTY(EditInstanceOnly, meta = (EditCondition = "bIsSpawningPosition"), Category = "Spawning Position")
	TArray<float> SpawningRotation;
	
	UPROPERTY()
	UArrowComponent* ArrowComponent = nullptr;

private:
	void GenerateNodePathPositionVisualisation();
	void ActivateArrowComponent();
};


