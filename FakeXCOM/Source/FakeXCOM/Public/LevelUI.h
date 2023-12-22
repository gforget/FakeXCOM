// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUI.generated.h"

class ATBTacticalGameMode;
class UTargetButton;
class UCanvasPanel;
class UOverlay;
class UCanvasPanelSlot;
class UUnitAbility;

UENUM(BlueprintType)
enum EStatusType
{
	Damage,
	CriticalDamage,
	Miss,
	OverWatch,
	Reloading,
	HunkerDown
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPUnitSpawnEvent, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPUnitSelectedEvent, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPUnitHealthChangeEvent, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPGunAmmoChangeEvent, AGun*, Gun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPTargetSelectedEvent, int, TargetIndex);
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
	void SubscribeToUnitEvent(AUnit* Unit);
	
	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPUnitSpawnEvent OnBPUnitSpawnEvent;

	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPUnitSelectedEvent OnBPUnitSelectedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPTargetSelectedEvent OnBPTargetSelectedEvent;
	
	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPAbilitySelectionModeChangeEvent OnBPAbilitySelectionModeChangeEvent;
	
	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPUnitHealthChangeEvent OnBPUnitHealthChangeEvent;

	UPROPERTY(BlueprintAssignable, Category = "Main Events")
	FBPGunAmmoChangeEvent OnBPGunAmmoChangeEvent;
	
	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void AddHealthBar(UOverlay* HealthBar, UCanvasPanelSlot* ReferencePanelSlot, UCanvasPanel* MainCanvas);

	UFUNCTION(BlueprintCallable, Category = "Main Functions")
	void FocusHealthBar(int IdUnit);
	
	UPROPERTY(BlueprintReadWrite)
	TMap<int, UOverlay*> HealthBarAssociationMap;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetBar Properties")
	UTexture2D* EnemyTargetImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetBar Properties")
	UTexture2D* EnemyTargetCritImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetBar Properties")
	UTexture2D* AllyTargetImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "HealtBar Properties")
	float UnfocusAlpha = 0.5f;

	UFUNCTION(BlueprintCallable, Category = "Main Events")
	void BPCallStatusEvent(AUnit* TargetUnit, EStatusType StatusType, float Number = -1.0f);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Main Events")
	void CallStatusEvent(AUnit* TargetUnit, EStatusType StatusType, float Number);
	
private:
	UFUNCTION()
	void OnTargetSelected(int TargetIndex);
	
	UFUNCTION()
	void OnUnitSelected(AUnit* Unit);

	UFUNCTION()
	void OnUnitSpawned(AUnit* Unit);

	UFUNCTION()
	void OnUnitHealthChange(AUnit* Unit);

	UFUNCTION()
	void OnGunAmmoChange(AGun* Gun);
	
	UFUNCTION()
	void OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue);
};


