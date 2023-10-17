// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelBlock.generated.h"

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
	
	void GenerateNodePathVisualisation();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pathfinder")
	TArray<FVector> NodePathPositions;
};


