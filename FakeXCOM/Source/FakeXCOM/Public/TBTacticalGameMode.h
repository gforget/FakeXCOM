// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LevelBlock.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LevelUI.h"
#include "TBTacticalGameMode.generated.h"

class ATBTacticalMainController;
class UTurnManager;
class UTilePathFinder;
class UFactionManager;
class UUI3DManager;
class UUnitManager;
class UUnitAbilityManager;

UCLASS()
class FAKEXCOM_API ATBTacticalGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	
public:

	ATBTacticalGameMode();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Global Object")
	UUI3DManager* UI3DManagerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Global Object")
	UFactionManager* FactionManagerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Global Object")
	UTurnManager* TurnManagerComponent;

	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	UUnitAbilityManager* UnitAbilityManager;
	
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

private:
	UPROPERTY()
	bool bAbilitySelectionMode = false;
};
