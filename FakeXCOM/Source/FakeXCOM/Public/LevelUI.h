// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUI.generated.h"

class ATBTacticalGameMode;
class UCanvasPanel;
class UOverlay;
class UCanvasPanelSlot;
class UUnitAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPUnitSpawnEvent, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPUnitSelectedEvent, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPAbilitySelectionModeChangeEvent, bool, AbilitySelectionModeValue);

UCLASS()
class FAKEXCOM_API ULevelUI : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY()
	ATBTacticalGameMode*  TBTacticalGameMode;

	UPROPERTY()
	APlayerController*  PlayerController;
	
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void Initialization();
	
	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPUnitSpawnEvent OnBPUnitSpawnEvent;

	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPUnitSelectedEvent OnBPUnitSelectedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPAbilitySelectionModeChangeEvent OnBPAbilitySelectionModeChangeEvent;
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void AddHealthBar(UOverlay* HealthBar, UCanvasPanelSlot* ReferencePanelSlot, UCanvasPanel* MainCanvas);

	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void FocusHealthBar(int IdUnit);
	
	UPROPERTY(BlueprintReadWrite)
	TMap<int, UOverlay*> HealthBarAssociationMap;

	UPROPERTY(EditDefaultsOnly, Category = "HealtBar Properties")
	float UnfocusAlpha = 0.5f;

	void CallPlayerAbilitySelected(UUnitAbility* UnitAbility);
	
private:
	
	UFUNCTION()
	void OnUnitSelected(AUnit* Unit);

	UFUNCTION()
	void OnUnitSpawned(AUnit* Unit);

	UFUNCTION()
	void OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue);
};


