// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionManager.h"
#include "TargetManager.h"
#include "Abilities/GameplayAbility.h"
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

USTRUCT(BlueprintType)
struct NO_API FUnitAbilityInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bIsDisabled;

	UPROPERTY(BlueprintReadWrite)
	bool bIsHidden;
	
	UPROPERTY(BlueprintReadWrite)
	float RangeValue;

	UPROPERTY(BlueprintReadWrite)
	float MinDamage;

	UPROPERTY(BlueprintReadWrite)
	float MaxDamage;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> AllAvailableTargets;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<AActor*, float> TargetsHitChances;

	UPROPERTY(BlueprintReadWrite)
	TMap<AActor*, float> TargetsCritChances;
	
	FUnitAbilityInfoStruct()
	{
		RangeValue = 0.0f;
		MinDamage = 0.0f;
		MaxDamage = 0.0f;
		AllAvailableTargets = TArray<AActor*>();
		TargetsHitChances = TMap<AActor*, float>();
		TargetsCritChances = TMap<AActor*, float>();
	}
	
	FUnitAbilityInfoStruct(
		float _RangeValue,
		float _MinDamageValue,
		float _MaxDamageValue,
		const TArray<AActor*>& _AllAvailableTargets,
		const TMap<AActor*, float>& _TargetsHitChances,
		const TMap<AActor*, float>& _TargetsCritChances
		)
	{
		RangeValue = _RangeValue;
		MinDamage = _MinDamageValue;
		MaxDamage = _MaxDamageValue;
		AllAvailableTargets = _AllAvailableTargets;
		TargetsHitChances = _TargetsHitChances;
		TargetsCritChances = _TargetsCritChances;
	}
};

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
	FString AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability Properties")
	FString AbilityDisabledText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bIsAOE = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	TEnumAsByte<EAbilityRange> AbilityRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	TEnumAsByte<ETargetType> TargetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	TArray<TEnumAsByte<EFactionRelation>> ValidTargetFactionRelation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	float RangeValue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bHasDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasDamage"), Category = "Ability Properties")
	float MinDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bHasDamage"), Category = "Ability Properties")
	float MaxDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bHasHitChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta = (EditCondition = "bHasHitChance"), Category = "Ability Properties")
	float HitChance = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bHasCritChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta = (EditCondition = "bHasCritChance"), Category = "Ability Properties")
	float CritChance = 0.0f;
	
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

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetHitChance(int IdUnit, AActor* Target, float NewHitChanceValue);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetCritChanceEvent(AUnit* Unit, AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetCritChance(int IdUnit, AActor* Target, float NewCritChanceValue);

	UFUNCTION(BlueprintPure, Category="Utility Functions")
	float GetRangeToTarget(AUnit* Unit, AActor* Target);

	UFUNCTION(BlueprintPure, Category="Utility Functions")
	float GetTargetHitChance(int IdUnit, AActor* Target);

	UFUNCTION(BlueprintPure, Category="Utility Functions")
	float GetTargetCritChance(int IdUnit, AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void CostAllActions(int IdUnit);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void CostActions(int IdUnit, float CostValue);
	
	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void ApplyDamage(AActor* Target, float DamageValue);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	bool RollDiceForHit(int IdUnitTryingToHit, AActor* Target);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	bool RollDiceForCrit(int IdUnitTryingToHit, AActor* Target);
	
	UFUNCTION(BlueprintPure, Category="Main Functions")
	ATBTacticalGameMode* GetTBTacticalGameMode();
	
	UPROPERTY(BlueprintReadWrite, category = "Data")
	TMap<int, FUnitAbilityInfoStruct> UnitAbilityInfos;

	void OnUnitSelected(int IdUnit);
	
};


