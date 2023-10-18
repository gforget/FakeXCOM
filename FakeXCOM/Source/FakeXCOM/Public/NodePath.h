// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NodePath.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UNodePath : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNodePath();

	void Initialize();
	
	// world location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NodePath Properties")
	FVector WorldLocation;
	
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

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
