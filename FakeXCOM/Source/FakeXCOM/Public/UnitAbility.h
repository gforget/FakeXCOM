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
	Range
};

UCLASS()
class FAKEXCOM_API UUnitAbility : public UGameplayAbility
{
	GENERATED_BODY()

public :

	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* ButtonImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	TEnumAsByte<EAbilityRange> AbilityRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	TEnumAsByte<ETargetType> TargetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	TArray<TEnumAsByte<EFaction>> ValidTargetFaction;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ini")
	void OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeArg);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ini")
	void OnTargetSelected(int TargetIndex);
    	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetRangeValue();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetDamageValue(AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetHitChanceValue(AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetCritChanceValue(AActor* TargetActor);
};
