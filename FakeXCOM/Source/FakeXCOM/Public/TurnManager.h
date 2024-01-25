﻿// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FactionManager.h"
#include "Components/ActorComponent.h"
#include "TurnManager.generated.h"

class ATBTacticalGameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnEndedDelegate, EFaction, CurrentSelectedFaction);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UTurnManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UTurnManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	UPROPERTY(BlueprintAssignable)
	FTurnEndedDelegate OnTurnStartedEvent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Faction Properties")
	TArray<TEnumAsByte<EFaction>> FactionTurnOrder;

	UFUNCTION(BlueprintCallable, Category="TurnManager Function")
	EFaction GetSelectedFaction();
	
	UFUNCTION(BlueprintCallable, Category="TurnManager Function")
	void EndTurn();

private:
	
	UPROPERTY()
	int SelectedFactionIndex = 0;

	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
};
