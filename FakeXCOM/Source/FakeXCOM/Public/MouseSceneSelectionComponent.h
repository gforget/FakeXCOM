﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MouseSceneSelectionComponent.generated.h"
class ATBTacticalGameMode;
class AUnit;
class UNodePath;
class UTilePathFinder;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UMouseSceneSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMouseSceneSelectionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mouse Scene Selection Properties")
	bool bDebugMouseLineTrace = false;
	
private:
	void LeftClickSelection();
	void RightClickSelection();
	
	AActor* SelectActorFromMousePosition(FVector& HitPosition, bool bDebugShowActorNameReturned = false) const;

	UPROPERTY()
	UInputComponent* InputComponentPtr;
	
	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY()
	UTilePathFinder* TilePathFinder;
	
	UPROPERTY()
	UNodePath* CurrentMouseOverNodePath;
	
};
