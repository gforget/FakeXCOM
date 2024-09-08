// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class UGunAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGunAmmoChangeDelegate, AGun*, Gun);

UCLASS()
class FAKEXCOM_API AGun : public AActor
{
	GENERATED_BODY()

public:
	AGun();

	UPROPERTY(BlueprintAssignable)
	FGunAmmoChangeDelegate OnGunAmmoChangeEvent;

	UFUNCTION(BlueprintCallable)
	void SetRange(float value);
	
	UFUNCTION(BlueprintPure)
	float GetRange();

	UFUNCTION(BlueprintCallable)
	void SetRangeAimValueCurve(FRuntimeFloatCurve value);
	
	UFUNCTION(BlueprintPure)
	FRuntimeFloatCurve GetRangeAimValueCurve();
	
	UFUNCTION(BlueprintCallable)
	void SetAmmo(int value);
	
	UFUNCTION(BlueprintPure)
	int GetAmmo();

	UFUNCTION(BlueprintCallable)
	void SetMaxAmmo(int value);
	
	UFUNCTION(BlueprintPure)
	int GetMaxAmmo();

	UFUNCTION(BlueprintCallable)
	void SetMinDamage(int value);
	
	UFUNCTION(BlueprintPure)
	int GetMinDamage();

	UFUNCTION(BlueprintCallable)
	void SetMaxDamage(int value);
	
	UFUNCTION(BlueprintPure)
	int GetMaxDamage();

	UFUNCTION(BlueprintCallable)
	void SetCritChance(float value);
	
	UFUNCTION(BlueprintPure)
	float GetCritChance();

	UFUNCTION(BlueprintCallable)
	void SetCritMultiplier(float value);
	
	UFUNCTION(BlueprintPure)
	float GetCritMultiplier();
	
protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Range = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes")
	FRuntimeFloatCurve RangeAimValueCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes", meta = (ClampMin = "0", UIMin = "0"))
	int Ammo = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes", meta = (ClampMin = "0", UIMin = "0"))
	int MaxAmmo = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes", meta = (ClampMin = "0", UIMin = "0"))
	int MinDamage = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes", meta = (ClampMin = "0", UIMin = "0"))
	int MaxDamage = 5;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float CritChance = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Gun Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CritMultiplier = 2.0f;
};
