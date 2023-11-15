// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUI.generated.h"

class ATBTacticalGameMode;
class UCanvasPanel;
class UProgressBar;
class UCanvasPanelSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitSpawnEvent, AUnit*, Unit);

UCLASS()
class FAKEXCOM_API ULevelUI : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY()
	ATBTacticalGameMode*  TBTacticalGameMode;

	UPROPERTY()
	APlayerController*  PlayerController;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FUnitSpawnEvent OnUnitSpawnEvent;
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void AddHealthBar(UProgressBar* HealthBar, UCanvasPanelSlot* ReferencePanelSlot, UCanvasPanel* MainCanvas);

	UPROPERTY(BlueprintReadWrite)
	TMap<int, UProgressBar*> HealthBarAssociationMap;
	
protected:
	
	// You can add variables here

	private:
	// You can add private functions or variables here
};
