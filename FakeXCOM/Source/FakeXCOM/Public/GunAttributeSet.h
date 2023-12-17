// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GunAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class FAKEXCOM_API UGunAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Range;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CritChance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CritMultiplier;
	
public:

	//Range
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGunAttributeSet, Range);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Range);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Range);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Range)
	
	//Ammo Attribute
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGunAttributeSet, Ammo);
	float GetAmmo() const;
	void SetAmmo(float NewVal) const;
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Ammo);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGunAttributeSet, MaxAmmo);
	float GetMaxAmmo() const;
	void SetMaxAmmo(float NewVal) const;
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxAmmo);
	
	//Damage Attributes
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGunAttributeSet, MinDamage);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MinDamage);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MinDamage);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MinDamage);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGunAttributeSet, MaxDamage);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxDamage);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxDamage);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxDamage);

	//Crit Attributes
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGunAttributeSet, CritChance);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(CritChance);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(CritChance);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(CritChance);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UGunAttributeSet, CritMultiplier);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(CritMultiplier);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(CritMultiplier);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(CritMultiplier);
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
