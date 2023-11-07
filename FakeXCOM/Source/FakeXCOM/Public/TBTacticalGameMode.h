// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LevelBlock.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TBTacticalGameMode.generated.h"

class UTilePathFinder;
class ASoldier;
/**
 * 
 */
UCLASS()
class FAKEXCOM_API ATBTacticalGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	
	UPROPERTY()
	UTilePathFinder* TilePathFinder;

	UPROPERTY()
	bool bInitialized = false;

	UPROPERTY(EditDefaultsOnly, Category = "Spawnable Unit")
	TSubclassOf<ASoldier> SoldierClass;
};
