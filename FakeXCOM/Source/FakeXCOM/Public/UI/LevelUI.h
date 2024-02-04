// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Containers/UnrealString.h"
#include "Managers/TurnManager.h"

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
	HunkerDown,
	Healing,
	CriticalHealing
};

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
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void OnBPUnitSpawnedEvent(AUnit* Unit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void OnBPUnitSelectedEvent(AUnit* Unit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void OnBPTargetSelectedEvent(int TargetIndex);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void OnBPAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void OnBPUnitHealthChangeEvent(AUnit* Unit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void OnBPGunAmmoChangeEvent(AGun* Gun);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void OnBPTurnStartedEvent(EFaction Faction);
	
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

	UFUNCTION(BlueprintCallable, Category = "Main Events")
	void BPCallCustomAlertStatusEvent(AUnit* TargetUnit, FColor Color, const FString& Text, float Number = -1.0f);

	UFUNCTION(BlueprintNativeEvent, Category = "Main Events")
	void CallCustomAlertStatusEvent(AUnit* TargetUnit, FColor Color, const FString& Text, float Number);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Main Events")
	void OnUnitIsDeadEvent(AUnit* DeadUnit);

	UFUNCTION(BlueprintNativeEvent, Category = "Main Events")
	void OnAllUnitFromFactionDeadEvent(EFaction Faction);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void SetControlInterfaceVisibility(bool Val);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Main Events")
	void ShowTargetImage(bool Val);
	
private:
	UFUNCTION()
	void OnTargetSelected(int TargetIndex, AUnit* SelectingUnit);
	
	UFUNCTION()
	void OnUnitSelected(AUnit* Unit);

	UFUNCTION()
	void OnUnitSpawned(AUnit* Unit);

	UFUNCTION()
	void OnUnitIsDead(AUnit* Unit);

	UFUNCTION()
	void OnAllUnitFromFactionDead(EFaction Faction);
	
	UFUNCTION()
	void OnUnitHealthChange(AUnit* Unit);

	UFUNCTION()
	void OnGunAmmoChange(AGun* Gun);

	UFUNCTION()
	void OnTurnStartedEvent(EFaction CurrentFaction);
	
	UFUNCTION()
	void OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue);
};


