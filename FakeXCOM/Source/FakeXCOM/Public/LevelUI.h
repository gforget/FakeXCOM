// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUI.generated.h"

class ATBTacticalGameMode;
class UCanvasPanel;
class UOverlay;
class UCanvasPanelSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitSpawnEvent, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitSelectedEvent, AUnit*, Unit);

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

	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FUnitSelectedEvent OnUnitSelectedEvent;
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void AddHealthBar(UOverlay* HealthBar, UCanvasPanelSlot* ReferencePanelSlot, UCanvasPanel* MainCanvas);

	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void FocusHealthBar(int IdUnit);
	
	UPROPERTY(BlueprintReadWrite)
	TMap<int, UOverlay*> HealthBarAssociationMap;

	UPROPERTY(EditDefaultsOnly, Category = "HealtBar Properties")
	float UnfocusAlpha = 0.5f;
	
protected:
	
	// You can add variables here

	private:
	// You can add private functions or variables here
};
