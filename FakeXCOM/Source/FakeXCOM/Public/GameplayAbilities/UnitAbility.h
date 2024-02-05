// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "Managers/FactionManager.h"
#include "Managers/TargetManager.h"

#include "UnitAbility.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EAbilityRange
{
	Melee,
	Range,
	RangeLineOfSight
};

class AGun;

UCLASS()
class FAKEXCOM_API UUnitAbility : public UGameplayAbility
{
	GENERATED_BODY()

public :

	UPROPERTY(BlueprintReadWrite, Category="Default Variables")
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Variables")
	UTexture2D* ButtonImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityId;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Properties")
	FString AbilityDisabledText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bIsAOE = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	TEnumAsByte<ETargetType> TargetType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "TargetType != ETargetType::Self"), Category = "Ability Properties")
	TEnumAsByte<EAbilityRange> AbilityRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "TargetType != ETargetType::Self"), Category = "Ability Properties")
	TEnumAsByte<EDeadTargetFilter> DeadTargetFilter = NoDeadTarget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "TargetType != ETargetType::Self"), Category = "Ability Properties")
	TArray<TEnumAsByte<EFactionRelation>> ValidTargetFactionRelation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "TargetType != ETargetType::Self"), Category = "Ability Properties")
	bool bUseDynamicRange;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "TargetType != ETargetType::Self && !bUseDynamicRange"), Category = "Ability Properties")
	float RangeValue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bHasDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bUseDynamicDamage;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bHasDamage && !bUseDynamicDamage"), Category = "Ability Properties")
	float MinDamage;
	
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bHasDamage && !bUseDynamicDamage"), Category = "Ability Properties")
	float MaxDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bHasHitChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bUseDynamicHitChance;
	
	UPROPERTY(EditDefaultsOnly,meta = (EditCondition = "bHasHitChance && !bUseDynamicHitChance"), Category = "Ability Properties")
	float HitChance = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bHasCritChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bUseDynamicCritChance;
	
	UPROPERTY(EditDefaultsOnly,meta = (EditCondition = "bHasCritChance && !bUseDynamicCritChance"), Category = "Ability Properties")
	float CritChance = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Properties")
	bool bUseTargetImage = false;
	
	void OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeRef, int IdUnit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enabling Ability Event")
	void SetAbilityEnabledEvent(AUnit* Unit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetIsDisabled(AUnit* Unit, bool Val);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	bool GetIsDisabled(AUnit* Unit);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enabling Ability Event")
	void SetAbilityHiddenEvent(AUnit* Unit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetIsHidden(AUnit* Unit, bool Val);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	bool GetIsHidden(AUnit* Unit);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetTargets(int IdUnit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void AddTargets(int IdOwningUnit, TArray<int> IdTargetUnits);
	
	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void RemoveTargets(int IdOwningUnit, TArray<int> IdTargetUnits);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void EndSetTargets(int IdOwningUnit);
	
	void SetAbilityPropertiesOnAssigned(int IdUnit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetDynamicRangeValueEvent(AUnit* Unit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetDynamicRangeValue(int IdUnit, float NewRangeValue);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	float GetDynamicRangeValue(int IdUnit);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetDynamicDamageValueEvent(AUnit* Unit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetDynamicDamageValue(int IdUnit, float NewMinDamageValue, float NewMaxDamageValue);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	float GetDynamicMinDamageValue(int IdUnit);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	float GetDynamicMaxDamageValue(int IdUnit);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetHitChanceEvent(AUnit* Unit, AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	float GetHitChance(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath);
	
	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetHitChance(int IdUnit, AActor* Target, float NewHitChanceValue);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetCritChanceEvent(AUnit* Unit, AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	float GetCritChance(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath);
	
	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetCritChance(int IdUnit, AActor* Target, float NewCritChanceValue);

	UFUNCTION(BlueprintPure, Category="Utility Functions")
	float GetRangeToTarget(AUnit* Unit, AActor* Target, UNodePath* UnitNodePath, UNodePath* TargetNodePath);

	UFUNCTION(BlueprintPure, Category="Utility Functions")
	float GetTargetHitChance(int IdUnit, AActor* Target);

	UFUNCTION(BlueprintPure, Category="Utility Functions")
	float GetTargetCritChance(int IdUnit, AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void CostAllActions(int IdUnit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void CostActions(int IdUnit, float CostValue);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void CostAmmo(AGun* Gun, float CostValue);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void RechargeAllAmmo(AGun* Gun);
	
	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void ApplyDamage(AActor* Target, float DamageValue, bool IsCrit = false);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void ApplyHeal(AActor* Target, float DamageValue, bool IsCrit = false);
	
	UFUNCTION(BlueprintPure, Category="Main Functions")
	bool RollDiceForHit(int IdUnitTryingToHit, AActor* Target);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	bool RollDiceForCrit(int IdUnitTryingToHit, AActor* Target);
	
	UFUNCTION(BlueprintPure, Category="Main Functions")
	ATBTacticalGameMode* GetTBTacticalGameMode();

	void OnUnitSelected(int IdUnit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void RotateTowardTarget(AUnit* Unit, AActor* Target);
};


