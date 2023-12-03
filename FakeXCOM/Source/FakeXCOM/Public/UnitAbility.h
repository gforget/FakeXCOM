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

UCLASS()
class FAKEXCOM_API UUnitAbility : public UGameplayAbility
{
	GENERATED_BODY()

public :

	UPROPERTY(BlueprintReadOnly, Category="Default")
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* ButtonImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	FString AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Properties")
	bool bIsAOE = false;
	
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
	
	void OnAbilityAssigned(ATBTacticalGameMode* TBTacticalGameModeRef);
	
	void OnTargetSelected(int TargetIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetTargets();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetAbilityPropertiesOnAssigned();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dynamic Value Event")
	void SetAbilityPropertiesBaseOnTargetSelected(AActor* TargetActor);
	
	UPROPERTY(BlueprintReadWrite, category = "Data")
	TArray<AActor*> AllAvailableTarget;
};
