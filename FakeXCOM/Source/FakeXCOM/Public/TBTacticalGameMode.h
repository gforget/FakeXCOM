// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LevelBlock.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LevelUI.h"
#include "TBTacticalGameMode.generated.h"

class UTilePathFinder;
class ATBTacticalMainController;
class UUI3DManager;
class UUnitManager;

UCLASS()
class FAKEXCOM_API ATBTacticalGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	
public:

	ATBTacticalGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UUI3DManager* UI3DManagerComponent;

	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	UUnitManager* UnitManager;
	
	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	UTilePathFinder* TilePathFinder;
	
	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	ATBTacticalMainController* MainController;

	UPROPERTY()
	bool bInitialized = false;

	UPROPERTY()
	FVector TopPosition = FVector(TNumericLimits<float>::Min(), TNumericLimits<float>::Min(), 0.0f);

	UPROPERTY()
	FVector BottomPosition = FVector(TNumericLimits<float>::Max(), TNumericLimits<float>::Max(), 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Level Properties")
	TSubclassOf<ULevelUI> LevelUIClass;

	UPROPERTY()
	ULevelUI* LevelUI;

	//TODO: will be put in the NodePath to spawn specific unit at a position
	UPROPERTY(EditDefaultsOnly, Category = "Spawnable Unit")
	TSubclassOf<AUnit> UnitClass;
	
};
