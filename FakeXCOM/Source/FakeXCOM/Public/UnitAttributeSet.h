// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "UnitAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class FAKEXCOM_API UUnitAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Actions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxActions;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxMoveDistancePerAction;
public:

	//Health Attribute
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UUnitAttributeSet, Health);
	float GetHealth() const;
	void SetHealth(float NewVal) const;
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UUnitAttributeSet, MaxHealth);
	float GetMaxHealth() const;
	void SetMaxHealth(float NewVal) const;
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxHealth);
	
	//Actions Attribute
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UUnitAttributeSet, Actions);
	float GetActions() const;
	void SetActions(float NewVal) const;
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Actions);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UUnitAttributeSet, MaxActions);
	float GetMaxActions() const;
	void SetMaxActions(float NewVal) const;
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxActions);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UUnitAttributeSet, MaxMoveDistancePerAction);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxMoveDistancePerAction);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxMoveDistancePerAction);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxMoveDistancePerAction);
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
