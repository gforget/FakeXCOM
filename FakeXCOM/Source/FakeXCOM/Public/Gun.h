// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class UGunAttributeSet;

UCLASS()
class FAKEXCOM_API AGun : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGun();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	FRuntimeFloatCurve RangeAimValueCurve;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	const UGunAttributeSet* GunAttributeSet;

protected:
	virtual void BeginPlay() override;
	
};
