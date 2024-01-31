// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FactionManager.generated.h"

UENUM(BlueprintType)
enum EFaction
{
	XCOM,
	Alien,
	Resistance,
	Civilian
};

UENUM(BlueprintType)
enum EFactionRelation
{
	Ally,
	Enemy,
	Neutral,
	OwnFaction
};

UENUM(BlueprintType)
enum EAssignedController
{
	PlayerController,
	AIController
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UFactionManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFactionManager();

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	EFactionRelation GetFactionRelation(EFaction FactionA, EFaction FactionB);

	TArray<EFaction> AllFaction = {EFaction::XCOM, EFaction::Alien, EFaction::Resistance, EFaction::Civilian};
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factions Properties")
	TMap<TEnumAsByte<EFaction>, TEnumAsByte<EAssignedController>> FactionsController;
};
