// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TBTacticalGameMode.generated.h"

class UTilePathFinder;
/**
 * 
 */
UCLASS()
class FAKEXCOM_API ATBTacticalGameMode : public AGameModeBase
{
	GENERATED_BODY()
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	UTilePathFinder* TilePathFinder; //will be define even if nothing is attach to it
};
