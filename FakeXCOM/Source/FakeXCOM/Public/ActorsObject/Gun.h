// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class UGunAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGunAmmoChangeDelegate, AGun*, Gun);

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

	UPROPERTY(BlueprintAssignable)
	FGunAmmoChangeDelegate OnGunAmmoChangeEvent;
	
	void CallAmmoChanged();
	
protected:
	virtual void BeginPlay() override;
	
};
