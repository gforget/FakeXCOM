// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "ActorsObject/LevelBlock.h"
#include "GameFramework/GameModeBase.h"
#include "UI/LevelUI.h"

#include "TBTacticalGameMode.generated.h"

class ATBTacticalMainController;
class UTargetManager;
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
	UTargetManager* TargetManager;
	
	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	UUnitAbilityManager* UnitAbilityManager;
	
	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	UUnitManager* UnitManager;
	
	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	UTilePathFinder* TilePathFinder;
	
	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	ATBTacticalMainController* MainController;

	UPROPERTY(BlueprintReadOnly, Category="Global Object")
	ULevelUI* LevelUIRef;
	
	UPROPERTY()
	bool bInitialized = false;
	
	UPROPERTY()
	FVector TopPosition = FVector(TNumericLimits<float>::Min(), TNumericLimits<float>::Min(), 0.0f);

	UPROPERTY()
	FVector BottomPosition = FVector(TNumericLimits<float>::Max(), TNumericLimits<float>::Max(), 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Level Properties")
	TSubclassOf<ULevelUI> LevelUIClass;

private:
	UPROPERTY()
	bool bAbilitySelectionMode = false;
};
